#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_calculateButton_clicked(); // Рассчитать BMR
    void on_addPersonButton_clicked(); // Добавить человека в таблицу
    void on_saveButton_clicked();      // Сохранить данные в файл

private:
    Ui::MainWindow *ui;

    double calculateBMR();             // Метод для расчета BMR
    void saveDataLocally();            // Сохранение данных локально
    void loadDataLocally();            // Загрузка данных из локального файла
};

#endif // MAINWINDOW_H
