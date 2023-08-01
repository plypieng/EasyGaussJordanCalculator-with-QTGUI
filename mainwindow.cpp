#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    matrix.resize(3, QVector<double>(4));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_calculationButton_clicked()
{
    matrix.clear();
    solution.clear();

    // Validate input fields
    for (int row = 0; row < 2; ++row) {
        for (int col = 0; col < 3; ++col) {
            QString valueStr = ui->matrixInputTable->item(row, col)->text();
            if (valueStr.isEmpty()) {
                QMessageBox::critical(this, "Error", "Please fill in all input fields.");
                return;
            }
        }
    }


    // Read the matrix values from the input fields
    for (int row = 0; row < 3; ++row) {
        QVector<double> rowData;
        for (int col = 0; col < 4; ++col) {
            QString valueStr = ui->matrixInputTable->item(row, col)->text();
            bool ok;
            double value = valueStr.toDouble(&ok);
            if (!ok) {
                QMessageBox::critical(this, "Error", "Invalid input at row " + QString::number(row + 1) +
                                                         " column " + QString::number(col + 1)) + ", plese use digits";
                return;
            }
            rowData.append(value);
        }
        matrix.append(rowData);
    }
    gaussJordanElimination();
    displaySolution();
}


/*
void MainWindow::on_calculationButton_clicked()
{
    matrix.clear();
    solution.clear();

    // Read the matrix values from the input fields
    for (int row = 0; row < 2; ++row) { // Update the loop condition to 2 instead of 3
        QVector<double> rowData;
        for (int col = 0; col < 3; ++col) { // Update the loop condition to 3 instead of 4
            QString valueStr = ui->matrixInputTable->item(row, col)->text();
            bool ok;
            double value = valueStr.toDouble(&ok);
            if (!ok || valueStr.isEmpty()) {
                QMessageBox::critical(this, "Error", "Invalid input at row " + QString::number(row + 1) +
                                                         " column " + QString::number(col + 1));
                return;
            }
            rowData.append(value);
        }
        matrix.append(rowData);
    }

    // Add an extra column for the augmented part with zeros
    for (int row = 0; row < 2; ++row) {
        matrix[row].append(0);
    }

    gaussJordanElimination();
    displaySolution();
}
*/


void MainWindow::gaussJordanElimination()
{
    int rowCount = matrix.size();
    int colCount = matrix[0].size();

    for (int pivot = 0; pivot < rowCount; ++pivot) {
        // Find the row with the largest pivot element
        int maxRow = pivot;
        for (int row = pivot + 1; row < rowCount; ++row) {
            if (qAbs(matrix[row][pivot]) > qAbs(matrix[maxRow][pivot]))
                maxRow = row;
        }

        // Swap the pivot row with the row with the largest pivot element
        if (maxRow != pivot) {
            for (int col = 0; col < colCount; ++col) {
                double temp = matrix[pivot][col];
                matrix[pivot][col] = matrix[maxRow][col];
                matrix[maxRow][col] = temp;
            }
        }

        // Normalize the pivot row
        double pivotValue = matrix[pivot][pivot];
        if(qFuzzyIsNull(pivotValue)) {
            //QMessageBox::critical(this, "Error", "Pivot value has been eliminated, unable to compute Gauss-Jordan elimination.");
            return;
        }
        for (int col = pivot; col < colCount; ++col)
            matrix[pivot][col] /= pivotValue;

        // Eliminate other rows
        for (int row = 0; row < rowCount; ++row) {
            if (row != pivot) {
                double factor = matrix[row][pivot];
                for (int col = pivot; col < colCount; ++col)
                    matrix[row][col] -= factor * matrix[pivot][col];
            }
        }

    }

}



int MainWindow::calculateMatrixRank(const QVector<QVector<double>>& matrix)
{
    int rank = 0;

    // check for all zero row
    for(const QVector<double>& rowData : matrix) {
        bool rowAllZero = true;
        for  (double value : rowData){
            if (!qFuzzyIsNull(value)){
                rowAllZero = false;
                break;
            }
        }
        if (!rowAllZero) rank++;
    }
    return rank;
}






void MainWindow::displaySolution()
{
    // Check the rank of the matrix
    int rank = calculateMatrixRank(matrix);

    // Extract the solution from the matrix
    solution.clear();
    for (const QVector<double>& rowData : matrix)
        solution.append(rowData.last());

    // Check for inconsistent system
    bool inconsistent = false;
    for (int row = 0; row < matrix.size(); ++row) {
        bool allCoefficientsZero = true;
        for (int col = 0; col < matrix[row].size() - 1; ++col) {
            if (!qFuzzyIsNull(matrix[row][col])) {
                allCoefficientsZero = false;
                break;
            }
        }
        if (allCoefficientsZero && !qFuzzyIsNull(matrix[row].last())) {
            inconsistent = true;
            break;
        }
    }

    QString solutionString;
    QStringList variableNames = {"x", "y", "z"};

    // Display the solution or error message
    if (!inconsistent) {
        if (rank == 0) {
            QMessageBox::critical(this, "Error", "The matrix is of rank 0, and no solution exists.");
            for (int i = 0; i < solution.size(); ++i) {
                solutionString += variableNames[i] + " = NaN \n";
            }
        } else if (rank == 1 || rank == 2) {
            QMessageBox::critical(this, "Error", "The matrix is of rank " + QString::number(rank) +
                                                     ", and infinite solutions exist.");
            for (int i = 0; i < solution.size(); ++i) {
                solutionString += variableNames[i] + " = NaN \n";
            }
        } else {
            for (int i = 0; i < solution.size(); ++i) {
                double value = solution[i];
                QString valueString;
                if (qAbs(value) >= 1e15 || qAbs(value) < 1e-15) {
                    valueString = QString::number(value, 'e', 5);
                } else {
                    valueString = QString::number(value, 'g', 15);
                }
                solutionString += variableNames[i] + " = " + valueString + "\n";
            }
        }
        ui->solutionLabel->setText(solutionString);
        ui->rankLabel->setText("System Rank: " + QString::number(rank));
    } else {
        QMessageBox::critical(this, "Error", "The system of equations is inconsistent. No valid solution exists.");
        for (int i = 0; i < solution.size(); ++i) {
            solutionString += variableNames[i] + " = NaN \n";
        }
        ui->solutionLabel->setText(solutionString);
        ui->rankLabel->setText("System inconsistent");
    }
}


/*void MainWindow::displaySolution()
{
    // Check the rank of the matrix
    int rank = calculateMatrixRank(matrix);

    // Extract the solution from the matrix
    solution.clear();
    for (const QVector<double>& rowData : matrix)
        solution.append(rowData.last());

    // Check for inconsistent system
    bool inconsistent = false;
    for (int row = 0; row < matrix.size(); ++row) {
        bool allCoefficientsZero = true;
        for (int col = 0; col < matrix[row].size() - 1; ++col) {
            if (!qFuzzyIsNull(matrix[row][col])) {
                allCoefficientsZero = false;
                break;
            }
        }
        if (allCoefficientsZero && !qFuzzyIsNull(matrix[row].last())) {
            inconsistent = true;
            break;
        }
    }

    QString solutionString;
    QStringList variableNames = {"x", "y", "z"};

    // Display the solution or error message
    if (inconsistent) {
        QMessageBox::critical(this, "Error", "The system of equations is inconsistent. No valid solution exists.");
        ui->rankLabel->setText("System inconsistent");
    } else {
        if (rank == 0) {
            QMessageBox::critical(this, "Error", "The matrix is of rank 0, and no solution exists.");
        } else if (rank == 1 || rank == 2) {
            QMessageBox::critical(this, "Error", "The matrix is of rank " + QString::number(rank) +
                                                     ", and infinite solutions exist.");
        } else {
            for (int i = 0; i < solution.size(); ++i) {
                solutionString += variableNames[i] + " = " + (qFuzzyIsNull(solution[i]) ? "NaN" : QString::number(solution[i])) + "\n";
            }
        }
        ui->solutionLabel->setText(solutionString);
        ui->rankLabel->setText("System Rank: " + QString::number(rank));
    }
}*/

