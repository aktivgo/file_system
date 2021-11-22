#include <iostream>
#include <Windows.h>
#include <vector>

using namespace std;

void printMenu();

wstring toRussian(wstring);

vector<wstring> getItemsName(HANDLE hFind, WIN32_FIND_DATAW wfd);

double getItemSize(const wstring&);

bool isDirectory(const wstring&);

double getDirectorySize(wstring);

void printDir(wstring);

void createFile(const wstring&);

void copyFile(const wstring&, const wstring&);

void deleteItem(wstring path);

void moveDirectory(const wstring&, const wstring&);

void setReadOnly(const wstring&);

void unsetReadOnly(const wstring&);

void checkHidden(const wstring&);

int main() {
    //system("chcp 65001");
    //SetConsoleCP(1251);
    //SetConsoleOutputCP(1251);
    setlocale(LC_CTYPE, "rus");

    int ch = -1;
    while (ch != 0) {
        printMenu();
        cout << "Выберите пункт меню: ";
        cin >> ch;
        switch (ch) {
        case 0:
            return EXIT_SUCCESS;
        case 1: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            cout << endl;
            printDir(path);
        }
              break;
        case 2: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            cout << endl;
            createFile(path);
        }
              break;
        case 3: {
            cout << "Введите путь до файла: ";
            wstring pathFrom;
            wcin >> pathFrom;
            cout << "Введите конечный путь: ";
            wstring pathTo;
            wcin >> pathTo;
            copyFile(pathFrom, pathTo);
        }
              break;
        case 4: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            deleteItem(path);
            cout << endl;
        }
              break;
        case 5: {
            cout << "Введите путь до объекта: ";
            wstring pathFrom;
            wcin >> pathFrom;
            cout << "Введите конечный путь: ";
            wstring pathTo;
            wcin >> pathTo;
            moveDirectory(pathFrom, pathTo);
        }
              break;
        case 6: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            setReadOnly(path);
        }
              break;
        case 7: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            unsetReadOnly(path);
        }
              break;
        case 8: {
            cout << "Введите путь: ";
            wstring path;
            wcin >> path;
            checkHidden(path);
        }
              break;
        default:
            cout << "\nПопробуйте ещё раз\n" << endl;
            break;
        }
    }

    return EXIT_SUCCESS;
}

void printMenu() {
    std::cout << "1. Отображение списка файлов и папок заданной директории с указанием их размера\n"
        << "2. Создание нового файла\n"
        << "3. Копирование файла\n"
        << "4. Удаление файла/директории\n"
        << "5. Переименование/перемещение директории\n"
        << "6. Установка атрибута «Только для чтения»\n"
        << "7. Снятие атрибута «Только для чтения»\n"
        << "8. Проверка атрибута «Скрытый»\n"
        << "0. Выход\n" << endl;
}

// Возвращает вектор с названиями элементов
vector<wstring> getItemsName(HANDLE hFind, WIN32_FIND_DATAW wfd) {
    vector<wstring> items;

    do {
        items.emplace_back(&wfd.cFileName[0]);
    } while (FindNextFileW(hFind, &wfd) != NULL);

    return items;
}

double getItemSize(const wstring& path) {
    if (isDirectory(path)) {
        return getDirectorySize(path);
    }

    HANDLE file = CreateFileW(path.c_str(),
        GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    LARGE_INTEGER fileSize = { 0 };
    GetFileSizeEx(file, &fileSize);
    CloseHandle(file);
    return (double)fileSize.QuadPart / 1024;
}

bool isDirectory(const wstring& path) {
    return GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES &&
        GetFileAttributesW(path.c_str()) & FILE_ATTRIBUTE_DIRECTORY;
}

double getDirectorySize(wstring path) {
    double sumSize = 0;

    WIN32_FIND_DATAW file;
    path += L"/*";
    HANDLE hFind = FindFirstFileW(path.c_str(), &file);
    FindNextFileW(hFind, &file);
    path.pop_back();

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if ((wstring)&file.cFileName[0] == L"." || (wstring)&file.cFileName[0] == L"..") {
                continue;
            }

            sumSize += getItemSize(path + &file.cFileName[0]);
        } while (FindNextFileW(hFind, &file) != NULL);
        FindClose(hFind);
    }

    return sumSize;
}

void printDir(wstring path) {
    if (!isDirectory(path)) {
        cout << "Ошибка: введите путь к директории\n" << endl;
        return;
    }
    if (path[path.length() - 1] != '/') {
        path = path.append(L"/");
    }
    WIN32_FIND_DATAW file;
    wstring findPath = path + L"*";
    auto const hFind = FindFirstFileW(findPath.c_str(), &file);

    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "Данный путь: ";
        wcout << path << endl;
        cout << "в системе не найден\n" << endl;
        return;
    }

    vector<wstring> items = getItemsName(hFind, file);
    for (auto& item : items) {
        if (item == L"." || item == L"..") {
            continue;
        }
        wcout << item << L" ";
        cout << getItemSize(path + item) << " KB" << endl;
    }
    FindClose(hFind);
    cout << endl;
}

void createFile(const wstring& path) {
    if (CreateFile(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)) {
        cout << "Успешно\n" << endl;
        return;
    }

    cout << "Ошибка\n" << endl;
}

void copyFile(const wstring& pathFrom, const wstring& pathTo) {
    if (CopyFile(pathFrom.c_str(), pathTo.c_str(), true)) {
        cout << "Успешно\n" << endl;
        return;
    }

    cout << "Ошибка\n" << endl;
}

void deleteItem(wstring path) {
    if (!isDirectory(path)) {
        DeleteFileW(path.c_str());
        return;
    }

    if (path[path.length() - 1] != '/') {
        path = path.append(L"/");
    }
    WIN32_FIND_DATAW file;
    wstring findPath = path + L"*";
    auto const hFind = FindFirstFileW(findPath.c_str(), &file);

    if (hFind == INVALID_HANDLE_VALUE) {
        cout << "Данный путь: ";
        wcout << path << endl;
        cout << "в системе не найден\n" << endl;
        return;
    }

    vector<wstring> items = getItemsName(hFind, file);

    for (auto& item : items) {
        if (item == L"." || item == L"..") {
            continue;
        }
        if (isDirectory(path + item)) {
            deleteItem(path + item);
        }
        else if (!DeleteFileW((path + item).c_str())) {
            cout << "Не удаётся удалить файл ";
            wcout << path + item << endl;
        }
    }

    if (!RemoveDirectoryW((path).c_str()))
    {
        cout << "Не удаётся удалить директорию ";
        wcout << path << endl;
    }

    FindClose(hFind);
}

void moveDirectory(const wstring& pathFrom, const wstring& pathTo) {
    if (!isDirectory(pathFrom)) {
        cout << "Это не директория\n" << endl;
        return;
    }
    if (MoveFileExW(pathFrom.c_str(), pathTo.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH)) {
        cout << "Успешно\n" << endl;
        return;
    }

    cout << "Ошибка\n" << endl;
}

void setReadOnly(const wstring& path) {
    if (SetFileAttributesW(path.c_str(), FILE_ATTRIBUTE_READONLY)) {
        cout << "Атрибут успешно установлен\n" << endl;
        return;
    }

    cout << "Не удалось установить атрибут\n" << endl;
}

void unsetReadOnly(const wstring& path) {
    if (SetFileAttributesW(path.c_str(), GetFileAttributesW(path.c_str()) & ~FILE_ATTRIBUTE_READONLY)) {
        cout << "Атрибут успешно снят\n" << endl;
        return;
    }

    cout << "Не удалось снять атрибут\n" << endl;
}

void checkHidden(const wstring& path) {
    if (GetFileAttributesW(path.c_str()) & FILE_ATTRIBUTE_HIDDEN) {
        cout << "Объект скрытый\n" << endl;
        return;
    }

    cout << "Объект не скрытый\n" << endl;
}