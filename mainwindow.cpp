#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // Добавляем формулы в ComboBox
    ui->formulaComboBox->addItem("Harris-Benedict Formula");
    ui->formulaComboBox->addItem("Mifflin-St Jeor Formula");

    // Настройка колонок таблицы
    ui->tableWidget->setColumnCount(7);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
                                               << "Full Name" << "Height (cm)" << "Weight (kg)" << "Age (years)"
                                               << "Gender" << "BMR (kcal/day)" << "Action");

    // Загрузка данных при запуске приложения
    loadDataLocally();
}

MainWindow::~MainWindow() {
    // Сохраняем данные при закрытии приложения
    saveDataLocally();
    delete ui;
}

double MainWindow::calculateBMR() {
    double weight = ui->weightSpinBox->value();
    double height = ui->heightSpinBox->value();
    int age = ui->ageSpinBox->value();
    QString gender = ui->genderComboBox->currentText();
    QString formula = ui->formulaComboBox->currentText();

    if (weight <= 0 || height <= 0 || age <= 0) {
        return 0.0;
    }

    double bmr = 0.0;

    if (formula == "Harris-Benedict Formula") {
        if (gender == "Male") {
            bmr = 88.36 + (13.4 * weight) + (4.8 * height) - (5.7 * age);
        } else {
            bmr = 447.6 + (9.2 * weight) + (3.1 * height) - (4.3 * age);
        }
    } else if (formula == "Mifflin-St Jeor Formula") {
        if (gender == "Male") {
            bmr = 10 * weight + 6.25 * height - 5 * age + 5;
        } else {
            bmr = 10 * weight + 6.25 * height - 5 * age - 161;
        }
    }

    return bmr;
}

void MainWindow::on_calculateButton_clicked() {
    double bmr = calculateBMR();

    if (bmr == 0.0) {
        ui->resultLabel->setText("Invalid input values.");
    } else {
        ui->resultLabel->setText(QString::number(bmr, 'f', 2) + " kcal/day");
    }
}

void MainWindow::on_addPersonButton_clicked() {
    double bmr = calculateBMR();

    if (bmr == 0.0) {
        QMessageBox::warning(this, "Error", "Invalid data.");
        return;
    }

    QString name = ui->lineEditName->text();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter a full name.");
        return;
    }

    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row, 0, new QTableWidgetItem(name));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(ui->heightSpinBox->value())));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(ui->weightSpinBox->value())));
    ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(ui->ageSpinBox->value())));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(ui->genderComboBox->currentText()));
    ui->tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(bmr, 'f', 2)));

    // Добавляем кнопку "Удалить"
    QPushButton *deleteButton = new QPushButton("Delete");
    connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
        ui->tableWidget->removeRow(row);
    });
    ui->tableWidget->setCellWidget(row, 6, deleteButton);
}

void MainWindow::on_saveButton_clicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save to Excel", "", "CSV Files (*.csv)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);

        for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
            QStringList rowData;
            for (int col = 0; col < ui->tableWidget->columnCount() - 1; ++col) { // Исключаем колонку с кнопками
                QTableWidgetItem *item = ui->tableWidget->item(row, col);
                if (item) {
                    rowData << item->text();
                } else {
                    rowData << "";
                }
            }
            out << rowData.join(",") << "\n";
        }

        file.close();
        QMessageBox::information(this, "Save", "Data successfully saved.");
    } else {
        QMessageBox::warning(this, "Error", "Failed to save the file.");
    }
}

void MainWindow::saveDataLocally() {
    QFile file("data.csv");

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Failed to save local data.");
        return;
    }

    QTextStream out(&file);

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < ui->tableWidget->columnCount() - 1; ++col) { // Исключаем колонку с кнопками
            QTableWidgetItem *item = ui->tableWidget->item(row, col);
            if (item) {
                rowData << item->text();
            } else {
                rowData << "";
            }
        }
        out << rowData.join(",") << "\n";
    }

    file.close();
}

void MainWindow::loadDataLocally() {
    QFile file("data.csv");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() == 6) {
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->insertRow(row);
            for (int col = 0; col < fields.size(); ++col) {
                ui->tableWidget->setItem(row, col, new QTableWidgetItem(fields[col]));
            }

            // Добавляем кнопку удаления
            QPushButton *deleteButton = new QPushButton("Delete");
            connect(deleteButton, &QPushButton::clicked, this, [this, row]() {
                ui->tableWidget->removeRow(row);
            });
            ui->tableWidget->setCellWidget(row, 6, deleteButton);
        }
    }

    file.close();
}
