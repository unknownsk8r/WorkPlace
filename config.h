#ifndef CONFIG
#define CONFIG

#include <QtGlobal> // QT_TRANSLATE_NOOP

namespace Config
{

const char applicationName[] = QT_TRANSLATE_NOOP("Config", "Workplace");

//! Версия приложения
const char applicationVersion[] = "1.0";
const char residenceFileNameFilter[] = QT_TRANSLATE_NOOP("Config", "ResidenceTable (*.tnb)");

// База данных.

//! Имя файла с базой данных рабочих.
const char employersDatabaseFileName[] = "employers.bin";
//! Путь к файлу с базой данных рабочих.
const char employersDatabasePath[] = "C:/Users/User/Documents/build-WorkPlace-Desktop_Qt_5_9_9_MSVC2015_32bit-Debug/employers.bin";

// Пользователи.

//! Имя файла с базой данных заданий.
const char tasksDatabaseFileName[] = "tasks.bin";
//! Путь к файла со всеми пользователями.
const char tasksDatabasePath[] = "C:/Users/User/Documents/build-WorkPlace-Desktop_Qt_5_9_9_MSVC2015_32bit-Debug/tasks.bin";

}

#endif // CONFIG
