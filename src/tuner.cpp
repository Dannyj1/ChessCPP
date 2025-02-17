#ifdef ZAGREUS_TUNER
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <cmath>
#include <ranges>

#include "board.h"
#include "eval.h"
#include "uci.h"
#include "tuner.h"

namespace Zagreus {
const int epochs = 100;
const int batchSize = 16;
const double learningRate = 0.01;
const int earlyStoppingPatience = 10;
int seed = 42;

double K = 1.0;

std::vector<std::vector<TunePosition>> createBatches(const std::vector<TunePosition>& positions) {
    std::vector<std::vector<TunePosition>> batches;
    batches.reserve((positions.size() + batchSize - 1) / batchSize);

    for (size_t i = 0; i < positions.size(); i += batchSize) {
        std::vector<TunePosition> batch;
        for (size_t j = i; j < std::min(i + batchSize, positions.size()); j++) {
            batch.push_back(positions[j]);
        }
        batches.push_back(batch);
    }

    return batches;
}

std::vector<TunePosition> loadPositions(const std::string& filePath, std::mt19937_64& gen, Board& board) {
    std::cout << "Loading positions..." << std::endl;
    std::vector<TunePosition> positions;
    std::vector<std::string> lines;
    std::ifstream fin(filePath);
    int win = 0;
    int loss = 0;
    int draw = 0;

    std::string line;
    while (std::getline(fin, line)) {
        lines.emplace_back(line);
    }

    positions.reserve(lines.size());

    for (std::string& posLine : lines) {
        if (posLine.empty() || posLine == " ") {
            continue;
        }

        double result;
        size_t c9_pos = posLine.find(" c9 ");
        if (c9_pos == std::string::npos) continue;

        std::string resultStr = posLine.substr(c9_pos + 4);
        std::string fen = posLine.substr(0, c9_pos);

        if (!board.setFromFEN(fen) || board.isDraw() ||
            board.isKingInCheck<WHITE>() || board.isKingInCheck<BLACK>()) {
            continue;
        }

        std::erase(resultStr, '"');
        std::erase(resultStr, ';');
        std::erase(resultStr, ' ');

        if (resultStr == "1" || resultStr == "1-0") {
            result = 1.0;
            win++;
        } else if (resultStr == "0" || resultStr == "0-1") {
            result = 0.0;
            loss++;
        } else {
            result = 0.5;
            draw++;
        }

        Evaluation eval{board};
        int evalScore = eval.evaluate();

        if (board.getSideToMove() == BLACK) {
            evalScore *= -1;
        }

        TunePosition tunePos{fen, result, evalScore};
        positions.emplace_back(tunePos);
    }

    // Reduce the biggest two classes to the size of the smallest class
    int smallestClassSize = std::min({win, loss, draw});
    std::vector<TunePosition> newPositions;
    int newWin = 0;
    int newLoss = 0;
    int newDraw = 0;

    std::ranges::shuffle(positions, gen);

    for (TunePosition& pos : positions) {
        if (pos.result == 1.0 && newWin < smallestClassSize) {
            newPositions.emplace_back(pos);
            newWin++;
        } else if (pos.result == 0.0 && newLoss < smallestClassSize) {
            newPositions.emplace_back(pos);
            newLoss++;
        } else if (pos.result == 0.5 && newDraw < smallestClassSize) {
            newPositions.emplace_back(pos);
            newDraw++;
        }

        if (newWin >= smallestClassSize && newLoss >= smallestClassSize &&
            newDraw >= smallestClassSize) {
            break;
        }
    }

    // Write all newPositions to a file by their fen strings
    std::ofstream fout("cleaned_positions.epd");
    for (TunePosition& pos : newPositions) {
        std::string resultStr;

        if (pos.result == 1.0) {
            resultStr = "1-0";
        } else if (pos.result == 0.0) {
            resultStr = "0-1";
        } else {
            resultStr = "1/2-1/2";
        }

        fout << pos.fen << " c9 \"" << resultStr << "\";" << std::endl;
    }

    fout.close();

    std::cout << "Loaded " << newPositions.size() << " positions." << std::endl;
    std::cout << "Win: " << newWin << ", Loss: " << newLoss << ", Draw: " << newDraw << std::endl;
    return newPositions;
}

double sigmoid(const double x) {
    return 1.0 / (1.0 + std::pow(10.0, -K * x / 400.0));
}

double sigmoidDerivative(const double x) {
    const double s = sigmoid(x);
    return (K * std::log(10.0) / 400.0) * s * (1.0 - s);
}

double loss(const double prediction, const double target) {
    return 0.5 * std::pow(prediction - target, 2);
}

double lossDerivative(const double prediction, const double target) {
    return prediction - target;
}

double findOptimalK(const std::vector<TunePosition>& positions) {
    const double invphi = (std::sqrt(5.0)-1)/2;
    const double invphi2 = (3 - std::sqrt(5.0))/2;
    double a = -2.0;
    double b = 2.0;
    constexpr double tol = 1e-4;

    auto averageLoss = [&positions](const double candidateK) -> double {
        double totalLoss = 0.0;
        const double oldK = K;
        K = candidateK;
        for (const auto& pos : positions) {
            const double prediction = sigmoid(pos.evalScore);

            totalLoss += loss(prediction, pos.result);
        }
        K = oldK;
        return totalLoss / static_cast<double>(positions.size());
    };

    double x1 = a + invphi2 * (b - a);
    double x2 = a + invphi * (b - a);
    double f1 = averageLoss(x1);
    double f2 = averageLoss(x2);

    double startingLoss = averageLoss(K);
    std::cout << "Starting loss: " << startingLoss << std::endl;

    while (b - a > tol) {
        if (f1 < f2) {
            // Minimum lies in [a, x2].
            b = x2;
            x2 = x1;
            f2 = f1;
            x1 = a + invphi2 * (b - a);
            f1 = averageLoss(x1);
        } else {
            // Minimum lies in [x1, b].
            a = x1;
            x1 = x2;
            f1 = f2;
            x2 = a + invphi * (b - a);
            f2 = averageLoss(x2);
        }
    }

    const double optimalK = (a + b) / 2.0;
    const double bestLoss = std::min(f1, f2);
    std::cout << "Best loss (K=" << optimalK << "): " << bestLoss << std::endl;
    return optimalK;
}

void gradientDescent(std::vector<TunePosition>& trainingSet, const std::vector<TunePosition>& validationSet, const std::vector<TunePosition>& testSet,
    std::mt19937_64& gen, Board& board) {
    std::vector<double> weights{};

    std::vector<double> bestWeights = weights;
    int epochsWithoutImprovement = 0;
    double bestValidationLoss = std::numeric_limits<double>::infinity();

    for (int epoch = 0; epoch < epochs; epoch++) {

    }
}

void startTuning(std::string filePath) {
    if (seed == 0) {
        std::random_device rd;
        seed = rd();
    }

    std::mt19937_64 gen = std::mt19937_64(seed);
    Engine engine{};
    // Make sure all values are initialized.
    engine.registerOptions();
    engine.doSetup();

    Board board{};
    std::vector<TunePosition> trainingSet = loadPositions(filePath, gen, board);
    if (trainingSet.empty()) {
        std::cout << "Error: No training positions loaded. Tuning cannot start." << std::endl;
        return;
    }

    std::vector<TunePosition> validationSet;
    std::vector<TunePosition> testSet;

    const int64_t validationSetSize = trainingSet.size() * 0.1;
    const int64_t testSetSize = trainingSet.size() * 0.1;

    validationSet.assign(trainingSet.begin(), trainingSet.begin() + validationSetSize);
    testSet.assign(trainingSet.begin() + validationSetSize, trainingSet.begin() + validationSetSize + testSetSize);
    trainingSet.erase(trainingSet.begin(), trainingSet.begin() + validationSetSize + testSetSize);

    std::cout << "Training set size: " << trainingSet.size() << std::endl;
    std::cout << "Validation set size: " << validationSet.size() << std::endl;
    std::cout << "Test set size: " << testSet.size() << std::endl;

    std::cout << "Finding the optimal K value..." << std::endl;
    K = findOptimalK(trainingSet);
    std::cout << "Optimal K value: " << K << std::endl;

    gradientDescent(trainingSet, validationSet, testSet, gen, board);
}
}
#endif
