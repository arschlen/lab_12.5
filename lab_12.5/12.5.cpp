#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

// Опис структури
struct Faculty {
    string name;
    vector<string> specialties;
    double passMark;
};

struct Institute {
    string name;
    string city;
    int admissionPlan;
    vector<Faculty> faculties;
};

// Функція запису масиву структур у файл
void WriteToFile(const string& filename, const vector<Institute>& institutes) {
    ofstream fout(filename, ios::binary | ios::app);
    if (!fout) {
        cerr << "Pomilka vidkrittya faylu dlya zapisu!" << endl;
        return;
    }
    for (const auto& inst : institutes) {
        size_t nameSize = inst.name.size();
        size_t citySize = inst.city.size();
        fout.write((char*)&nameSize, sizeof(nameSize));
        fout.write(inst.name.c_str(), nameSize);
        fout.write((char*)&citySize, sizeof(citySize));
        fout.write(inst.city.c_str(), citySize);
        fout.write((char*)&inst.admissionPlan, sizeof(inst.admissionPlan));
        size_t facultyCount = inst.faculties.size();
        fout.write((char*)&facultyCount, sizeof(facultyCount));
        for (const auto& fac : inst.faculties) {
            size_t facNameSize = fac.name.size();
            fout.write((char*)&facNameSize, sizeof(facNameSize));
            fout.write(fac.name.c_str(), facNameSize);
            size_t specialtiesCount = fac.specialties.size();
            fout.write((char*)&specialtiesCount, sizeof(specialtiesCount));
            for (const auto& spec : fac.specialties) {
                size_t specSize = spec.size();
                fout.write((char*)&specSize, sizeof(specSize));
                fout.write(spec.c_str(), specSize);
            }
            fout.write((char*)&fac.passMark, sizeof(fac.passMark));
        }
    }
    fout.close();
}

// Функція читання даних з файлу
vector<Institute> ReadFromFile(const string& filename) {
    ifstream fin(filename, ios::binary);
    if (!fin) {
        cerr << "Pomilka vidkrittya faylu dlya chytannya!" << endl;
        return {};
    }
    vector<Institute> institutes;
    while (fin.peek() != EOF) {
        Institute inst;
        size_t nameSize, citySize, facultyCount;
        fin.read((char*)&nameSize, sizeof(nameSize));
        inst.name.resize(nameSize);
        fin.read(&inst.name[0], nameSize);
        fin.read((char*)&citySize, sizeof(citySize));
        inst.city.resize(citySize);
        fin.read(&inst.city[0], citySize);
        fin.read((char*)&inst.admissionPlan, sizeof(inst.admissionPlan));
        fin.read((char*)&facultyCount, sizeof(facultyCount));
        for (size_t i = 0; i < facultyCount; ++i) {
            Faculty fac;
            size_t facNameSize, specialtiesCount;
            fin.read((char*)&facNameSize, sizeof(facNameSize));
            fac.name.resize(facNameSize);
            fin.read(&fac.name[0], facNameSize);
            fin.read((char*)&specialtiesCount, sizeof(specialtiesCount));
            for (size_t j = 0; j < specialtiesCount; ++j) {
                string spec;
                size_t specSize;
                fin.read((char*)&specSize, sizeof(specSize));
                spec.resize(specSize);
                fin.read(&spec[0], specSize);
                fac.specialties.push_back(spec);
            }
            fin.read((char*)&fac.passMark, sizeof(fac.passMark));
            inst.faculties.push_back(fac);
        }
        institutes.push_back(inst);
    }
    fin.close();
    return institutes;
}

// Функція виведення міст з політехнічними інститутами
void ListPolytechnicCities(const vector<Institute>& institutes) {
    cout << "Mista z politekhnichnymy instytutamy:" << endl;
    for (const auto& inst : institutes) {
        if (inst.name.find("Polytechnic") != string::npos) {
            cout << inst.city << endl;
        }
    }
}

// Функція пошуку за спеціальністю
void SearchBySpecialty(const vector<Institute>& institutes, const string& specialty) {
    cout << "Instytuty ta fakulty, shcho pryymaiut na spetsialnist '" << specialty << "':" << endl;
    for (const auto& inst : institutes) {
        for (const auto& fac : inst.faculties) {
            if (find(fac.specialties.begin(), fac.specialties.end(), specialty) != fac.specialties.end()) {
                cout << "Instytut: " << inst.name << ", Fakul'tet: " << fac.name << endl;
            }
        }
    }
}

// Функція пошуку найвищого прохідного бала
void FindHighestPassMark(const vector<Institute>& institutes, const string& specialty) {
    string bestInstitute, bestCity, bestFaculty;
    double highestPassMark = -1;
    for (const auto& inst : institutes) {
        for (const auto& fac : inst.faculties) {
            if (find(fac.specialties.begin(), fac.specialties.end(), specialty) != fac.specialties.end()) {
                if (fac.passMark > highestPassMark) {
                    highestPassMark = fac.passMark;
                    bestInstitute = inst.name;
                    bestCity = inst.city;
                    bestFaculty = fac.name;
                }
            }
        }
    }
    if (highestPassMark > 0) {
        cout << "Najvyshchyy prokhidnyy bal: " << highestPassMark << endl;
        cout << "Instytut: " << bestInstitute << ", Fakul'tet: " << bestFaculty << ", Misto: " << bestCity << endl;
    }
    else {
        cout << "Spetsialnist ne znaydena." << endl;
    }
}

// Меню програми
void Menu() {
    string filename;
    cout << "Vvedit imya faylu: ";
    cin >> filename;

    vector<Institute> institutes;
    int choice;
    do {
        cout << "\nMenyu:\n"
            << "1. Zapysaty danni u fayl\n"
            << "2. Perehlyanuty danni z faylu\n"
            << "3. Spysok mist z politekhnichnymy instytutamy\n"
            << "4. Shukaty instytuty za spetsialnistyu\n"
            << "5. Znayty najvyshchyy prokhidnyy bal\n"
            << "0. Vykhid\n"
            << "Vash vybir: ";
        cin >> choice;

        switch (choice) {
        case 1:
            // Заповнення даних вручну
            institutes = { { "Kyiv Polytechnic", "Kyiv", 500, {{"IT Faculty", {"IT", "Cyber"}, 180}} } };
            WriteToFile(filename, institutes);
            break;
        case 2:
            institutes = ReadFromFile(filename);
            for (const auto& inst : institutes) {
                cout << "Instytut: " << inst.name << ", Misto: " << inst.city << endl;
            }
            break;
        case 3:
            institutes = ReadFromFile(filename);
            ListPolytechnicCities(institutes);
            break;
        case 4: {
            string specialty;
            cout << "Vvedit spetsialnist: ";
            cin >> specialty;
            institutes = ReadFromFile(filename);
            SearchBySpecialty(institutes, specialty);
            break;
        }
        case 5: {
            string specialty;
            cout << "Vvedit spetsialnist: ";
            cin >> specialty;
            institutes = ReadFromFile(filename);
            FindHighestPassMark(institutes, specialty);
            break;
        }
        case 0:
            cout << "Do pobachennya!" << endl;
            break;
        default:
            cout << "Nekorektnyy vybir. Sprobuyte shche raz." << endl;
        }
    } while (choice != 0);
}

int main() {
    Menu();
    return 0;
}
