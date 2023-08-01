#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTableWidgetItem>
#include <Qlist>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_calculationButton_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QVector<double>> matrix;
    QVector<double> solution;

    void gaussJordanElimination();
    void displaySolution();

    int calculateMatrixRank(const QVector<QVector<double>>& matrix);


};

#endif // MAINWINDOW_H
