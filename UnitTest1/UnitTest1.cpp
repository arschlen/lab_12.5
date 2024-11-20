#include "pch.h"
#include "CppUnitTest.h"
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Опис структур
struct Faculty {
    std::string name;
    std::vector<std::string> specialties;
    double passMark;
};

struct Institute {
    std::string name;
    std::string city;
    int admissionPlan;
    std::vector<Faculty> faculties;
};

// Реалізація функцій
void WriteToFile(const std::string& filename, const std::vector<Institute>& institutes) {
    std::ofstream fout(filename, std::ios::binary);
    for (const auto& inst : institutes) {
        size_t nameSize = inst.name.size();
        fout.write((char*)&nameSize, sizeof(nameSize));
        fout.write(inst.name.c_str(), nameSize);

        size_t citySize = inst.city.size();
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

std::vector<Institute> ReadFromFile(const std::string& filename) {
    std::ifstream fin(filename, std::ios::binary);
    std::vector<Institute> institutes;
    while (fin.peek() != EOF) {
        Institute inst;
        size_t nameSize;
        fin.read((char*)&nameSize, sizeof(nameSize));
        inst.name.resize(nameSize);
        fin.read(&inst.name[0], nameSize);

        size_t citySize;
        fin.read((char*)&citySize, sizeof(citySize));
        inst.city.resize(citySize);
        fin.read(&inst.city[0], citySize);

        fin.read((char*)&inst.admissionPlan, sizeof(inst.admissionPlan));

        size_t facultyCount;
        fin.read((char*)&facultyCount, sizeof(facultyCount));
        for (size_t i = 0; i < facultyCount; ++i) {
            Faculty fac;
            size_t facNameSize;
            fin.read((char*)&facNameSize, sizeof(facNameSize));
            fac.name.resize(facNameSize);
            fin.read(&fac.name[0], facNameSize);

            size_t specialtiesCount;
            fin.read((char*)&specialtiesCount, sizeof(specialtiesCount));
            for (size_t j = 0; j < specialtiesCount; ++j) {
                std::string spec;
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

// Юніт-тест
namespace UnitTest1
{
    TEST_CLASS(UnitTest1)
    {
    public:

        TEST_METHOD(TestWriteAndReadFromFile)
        {
            const std::string testFile = "test_data.bin";

            // Тестові дані
            std::vector<Institute> inputData = {
                {
                    "Kyiv Polytechnic", "Kyiv", 500,
                    {
                        {"IT Faculty", {"IT", "Cybersecurity"}, 180.5},
                        {"Mechanical Faculty", {"Engineering"}, 150.0}
                    }
                },
                {
                    "Lviv Polytechnic", "Lviv", 300,
                    {
                        {"Architecture Faculty", {"Architecture", "Design"}, 160.0}
                    }
                }
            };

            // Запис до файлу
            WriteToFile(testFile, inputData);

            // Читання з файлу
            std::vector<Institute> outputData = ReadFromFile(testFile);

            // Перевірка даних
            Assert::AreEqual(inputData.size(), outputData.size(), L"Кількість інститутів має співпадати");
            Assert::AreEqual(inputData[0].name, outputData[0].name, L"Назви інститутів мають співпадати");
            Assert::AreEqual(inputData[0].city, outputData[0].city, L"Міста інститутів мають співпадати");

            // Очистка файлу
            std::remove(testFile.c_str());
        }
    };
}