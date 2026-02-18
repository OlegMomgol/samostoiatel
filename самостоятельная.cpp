// ConsoleApplication6.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

struct Operation {
    int id;
    string type;        
    string category;
    double amount;
    string date;        
    string description;
};


vector<Operation> operations;
int nextId = 1;
const string DATA_FILE = "finance_data.csv";

void showMainMenu();
void addOperation();
void viewOperations();
void deleteOperation();
void showStatistics();
void saveToFile();
void loadFromFile();
void clearInput();
string getCurrentDate();
bool isValidDate(const string& date);
void pause();

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

string getCurrentDate() {
    auto now = chrono::system_clock::now();
    time_t tt = chrono::system_clock::to_time_t(now);
    tm local_tm = *localtime(&tt);

    ostringstream oss;
    oss << put_time(&local_tm, "%Y-%m-%d");
    return oss.str();
}
 
bool isValidDate(const string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    try {
        int year = stoi(date.substr(0, 4));
        int month = stoi(date.substr(5, 2));
        int day = stoi(date.substr(8, 2));

        if (year < 1900 || year > 2100) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;

        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (day > (isLeap ? 29 : 28)) return false;
        }

        return true;
    }
    catch (...) {
        return false;
    }
}

void pause() {
    cout << "\nНажмите Enter для продолжения";
    clearInput();
}

void showMainMenu() {
    cout << "\n Учет финансов\n";
    cout << "1. Добавить операцию\n";
    cout << "2. Просмотр операций\n";
    cout << "3. Удалить операцию\n";
    cout << "4. Статистика\n";
    cout << "5. Сохранить в файл\n";
    cout << "6. Загрузить из файла\n";
    cout << "0. Выход\n";
    cout << "Выберите действие: ";
}


void addOperation() {
    Operation op;
    op.id = nextId++;

    cout << "\n Добавление новой операции \n";

    int typeChoice;
    do {
        cout << "Тип операции (1 - доход, 2 - расход): ";
        cin >> typeChoice;
        if (cin.fail() || (typeChoice != 1 && typeChoice != 2)) {
            cout << "Ошибка! Введите 1 или 2\n";
            clearInput();
        }
    } while (typeChoice != 1 && typeChoice != 2);

    op.type = (typeChoice == 1) ? "доход" : "расход";

    cout << "Категория (например: еда, транспорт, зарплата): ";
    clearInput();
    getline(cin, op.category);

    do {
        cout << "Сумма: ";
        cin >> op.amount;
        if (cin.fail() || op.amount <= 0) {
            cout << "Ошибка Введите положительное число\n";
            clearInput();
        }
    } while (op.amount <= 0);

    int dateChoice;
    cout << "Использовать текущую дату? (1 - да, 2 - ввести вручную): ";
    cin >> dateChoice;

    if (dateChoice == 1) {
        op.date = getCurrentDate();
        cout << "Установлена дата: " << op.date << endl;
    }
    else {
        clearInput();
        do {
            cout << "Введите дату (ГГГГ-ММ-ДД): ";
            getline(cin, op.date);
            if (!isValidDate(op.date)) {
                cout << "Ошибка! Неверный формат даты\n";
            }
        } while (!isValidDate(op.date));
    }

    cout << "Описание (необязательно): ";
    clearInput();
    getline(cin, op.description);

    operations.push_back(op);
    cout << "\nОперация успешно добавлена! ID: " << op.id << endl;
}

void viewOperations() {
    if (operations.empty()) {
        cout << "\nНет записей для отображения.\n";
        pause();
        return;
    }

    int choice;
    cout << "\n Просмотр операций \n";
    cout << "1. Все операции\n";
    cout << "2. Только доходы\n";
    cout << "3. Только расходы\n";
    cout << "4. За период\n";
    cout << "Выберите фильтр: ";
    cin >> choice;

    vector<Operation> filtered;
    string type;
    string startDate, endDate;

    switch (choice) {
    case 1:
        filtered = operations;
        break;
    case 2:
        type = "доход";
        break;
    case 3:
        type = "расход";
        break;
    case 4:
        clearInput();
        cout << "Введите начальную дату (ГГГГ-ММ-ДД): ";
        getline(cin, startDate);
        cout << "Введите конечную дату (ГГГГ-ММ-ДД): ";
        getline(cin, endDate);
        break;
    default:
        cout << "Неверный выбор\n";
        return;
    }

    if (choice == 2 || choice == 3) {
        for (const auto& op : operations) {
            if (op.type == type) {
                filtered.push_back(op);
            }
        }
    }

    if (choice == 4) {
        for (const auto& op : operations) {
            if (op.date >= startDate && op.date <= endDate) {
                filtered.push_back(op);
            }
        }
    }

    if (filtered.empty()) {
        cout << "\nНет операций, соответствующих критериям.\n";
        pause();
        return;
    }

    cout << "\n" << string(100, '-') << endl;
    cout << left << setw(6) << "ID"
        << setw(10) << "Тип"
        << setw(15) << "Категория"
        << setw(12) << "Сумма"
        << setw(12) << "Дата"
        << "Описание" << endl;
    cout << string(100, '-') << endl;

    for (const auto& op : filtered) {
        cout << left << setw(6) << op.id
            << setw(10) << op.type
            << setw(15) << op.category
            << right << setw(10) << fixed << setprecision(2) << op.amount << " руб."
            << left << " " << setw(10) << op.date
            << " " << op.description << endl;
    }
    cout << string(100, '-') << endl;

    pause();
}


void deleteOperation() {
    if (operations.empty()) {
        cout << "\nНет записей для удаления.\n";
        pause();
        return;
    }

    int id;
    cout << "\nВведите ID операции для удаления: ";
    cin >> id;

    auto it = find_if(operations.begin(), operations.end(),
        [id](const Operation& op) { return op.id == id; });

    if (it != operations.end()) {
        cout << "\nНайдена операция:\n";
        cout << "ID: " << it->id << ", Тип: " << it->type
            << ", Категория: " << it->category
            << ", Сумма: " << it->amount << " руб.\n";
        cout << "Дата: " << it->date << ", Описание: " << it->description << endl;

        char confirm;
        cout << "Удалить? (д/н): ";
        cin >> confirm;

        if (confirm == 'д' || confirm == 'Д') {
            operations.erase(it);
            cout << "Операция удалена.\n";
        }
        else {
            cout << "Удаление отменено.\n";
        }
    }
    else {
        cout << "Операция с ID " << id << " не найдена.\n";
    }

    pause();
}

void showStatistics() {
    if (operations.empty()) {
        cout << "\nНет данных для статистики.\n";
        pause();
        return;
    }

    double totalIncome = 0, totalExpense = 0;
    vector<pair<string, double>> incomeByCategory, expenseByCategory;

    for (const auto& op : operations) {
        if (op.type == "доход") {
            totalIncome += op.amount;
            incomeByCategory.push_back({ op.category, op.amount });
        }
        else {
            totalExpense += op.amount;
            expenseByCategory.push_back({ op.category, op.amount });
        }
    }

    cout << "\n Статистика \n";
    cout << fixed << setprecision(2);
    cout << "Общий доход:     " << totalIncome << " руб.\n";
    cout << "Общий расход:    " << totalExpense << " руб.\n";
    cout << "Текущий баланс:  " << (totalIncome - totalExpense) << " руб.\n";

    if (!incomeByCategory.empty()) {
        cout << "\n Доходы по категориям \n";
        vector<pair<string, double>> grouped;
        for (const auto& item : incomeByCategory) {
            auto it = find_if(grouped.begin(), grouped.end(),
                [&item](const pair<string, double>& p) { return p.first == item.first; });
            if (it != grouped.end()) {
                it->second += item.second;
            }
            else {
                grouped.push_back(item);
            }
        }

        for (const auto& cat : grouped) {
            double percent = (cat.second / totalIncome) * 100;
            cout << left << setw(20) << cat.first
                << right << setw(10) << cat.second << " руб. "
                << "(" << setprecision(1) << percent << "%)\n";
        }
    }

    if (!expenseByCategory.empty()) {
        cout << "\n Расходы по категориям \n";
        vector<pair<string, double>> grouped;
        for (const auto& item : expenseByCategory) {
            auto it = find_if(grouped.begin(), grouped.end(),
                [&item](const pair<string, double>& p) { return p.first == item.first; });
            if (it != grouped.end()) {
                it->second += item.second;
            }
            else {
                grouped.push_back(item);
            }
        }

        for (const auto& cat : grouped) {
            double percent = (cat.second / totalExpense) * 100;
            cout << left << setw(20) << cat.first
                << right << setw(10) << cat.second << " руб. "
                << "(" << setprecision(1) << percent << "%)\n";
        }
    }

    pause();
}

void saveToFile() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "\nОшибка открытия файла для сохранения\n";
        pause();
        return;
    }

    file << "ID;Type;Category;Amount;Date;Description\n";

    for (const auto& op : operations) {
        file << op.id << ";"
            << op.type << ";"
            << op.category << ";"
            << op.amount << ";"
            << op.date << ";"
            << op.description << "\n";
    }

    file.close();
    cout << "\nДанные успешно сохранены в файл " << DATA_FILE << endl;
    pause();
}

void loadFromFile() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "\nФайл " << DATA_FILE << " не найден. Будет создан новый при сохранении.\n";
        pause();
        return;
    }

    operations.clear();
    string line;

    getline(file, line);

    int maxId = 0;
    while (getline(file, line)) {
        istringstream ss(line);
        Operation op;
        string field;

        try {
            getline(ss, field, ';');
            op.id = stoi(field);

            getline(ss, op.type, ';');
            getline(ss, op.category, ';');

            getline(ss, field, ';');
            op.amount = stod(field);

            getline(ss, op.date, ';');
            getline(ss, op.description, ';');

            operations.push_back(op);
            maxId = max(maxId, op.id);
        }
        catch (const exception& e) {
            cout << "Ошибка чтения строки: " << line << endl;
        }
    }

    file.close();
    nextId = maxId + 1;

    cout << "\nЗагружено " << operations.size() << " операций из файла " << DATA_FILE << endl;
    pause();
}

int main() {
    setlocale(LC_ALL, "ru");

    cout << "Учет финансов \n";

    loadFromFile();

    int choice;
    do {
        showMainMenu();
        cin >> choice;

        if (cin.fail()) {
            cout << "Ошибка! Введите число.\n";
            clearInput();
            continue;
        }

        switch (choice) {
        case 1:
            addOperation();
            break;
        case 2:
            viewOperations();
            break;
        case 3:
            deleteOperation();
            break;
        case 4:
            showStatistics();
            break;
        case 5:
            saveToFile();
            break;
        case 6:
            loadFromFile();
            break;
        case 0:
            cout << "\nСохранить данные перед выходом? (1 - да, 0 - нет): ";
            int saveChoice;
            cin >> saveChoice;
            if (saveChoice == 1) {
                saveToFile();
            }
            cout << "Программа завершена\n";
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
