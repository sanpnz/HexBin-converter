#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
#endif

#define BUFFER_SIZE (1024*1024) // 1mb

// Функция вывода help информации
void PrintHelp()
{
    printf("\nПрограмма конвертации файла ASCII HEX в бинарное содержимое и наоборот\n");
    printf("Использование:\n");
    #ifdef _WIN32
    printf("./conv.exe -a file-name.hex   Конвертирует входной HEX файл в выходной бинарный файл с именем \"file-name.hex.bin\"\n");
    printf("./conv.exe -b file-name.bin   Конвертирует входной бинарный файл в выходной файл в формате HEX с именем \"file-name.bin.hex\"\n");
    #else
    printf("./conv -a file-name.hex   Конвертирует входной HEX файл в выходной бинарный файл с именем \"file-name.hex.bin\"\n");
    printf("./conv -b file-name.bin   Конвертирует входной бинарный файл в выходной файл в формате HEX с именем \"file-name.bin.hex\"\n");
    #endif
}

// Функция конвертации HEX символа в бинарный
// [in] cSymbol    конвертируемый HEX символ
// result          Bin символ
int ConvertSymbolHexToBin(char cSymbol)
{
    if (cSymbol >= '0' && cSymbol <= '9') 
    {   
        return cSymbol - '0';
    }

    if (cSymbol >= 'A' && cSymbol <= 'F') 
    {
        return cSymbol - 'A' + 10;
    }

    if (cSymbol >= 'a' && cSymbol <= 'f') 
    {
        return cSymbol - 'a' + 10;
    }
    
    return -1;
}

// Функция конвертации Bin файла в HEX
// [in] pcInFileName    имя входнохо файла
// [in] pcOutFileName   имя выходного файла
// result               результат выполнения 0 - Успех, 1 - Неудача
int ConvertFileBinToHex(const char *pcInFileName, const char *pcOutFileName) 
{
    int nResult = 0;

    // Открытие файла на чтение
    FILE *pInputFile = fopen(pcInFileName, "rb");
    if(!pInputFile)
    {
        perror("\nНе удалось открыть файл для чтения");
        return 1;
    }

    // Открытие файла на запись
    FILE *pOutputFile = fopen(pcOutFileName, "w");
    if(!pOutputFile)
    {
        perror("\nНе удалось открыть файл для записи\n");
        fclose(pInputFile);
        return 1;
    }

    // Буфер под данные
    char *pcBuffer = (char *)malloc(BUFFER_SIZE);
    if(!pcBuffer)
    {
        printf("\nОшибка. Ошибка выделения памяти для буфера\n");
        fclose(pInputFile);
        fclose(pOutputFile);
        remove(pcOutFileName);
        return 1;
    }
   
    // Количество прочитанных байт
    size_t ulBytesRead;
    while ((ulBytesRead = fread(pcBuffer, 1, BUFFER_SIZE, pInputFile)) > 0) 
    {
        for (size_t i = 0; i < ulBytesRead; i++) 
        {
            if (fprintf(pOutputFile, "%02X", pcBuffer[i]) < 0)
            {
                printf("\nОшибка. Ошибка при записи в файл HEX\n");
                free(pcBuffer);
                fclose(pInputFile);
                fclose(pOutputFile);
                remove(pcOutFileName);
                return 1;
            }
        }
    }
 
    // Освобождение ресурсов
    free(pcBuffer);
    fclose(pInputFile);
    fclose(pOutputFile);
    
    return nResult;
}

// Функция конвертации HEX файла в Bin
// [in] pcInFileName    имя входнохо файла
// [in] pcOutFileName   имя выходного файла
// result               результат выполнения 0 - Успех, 1 - Неудача
int ConvertFileHexToBin(const char *pcInFileName, const char *pcOutFileName) 
{
    int nResult = 0;

    // Открытие файла на чтение
    FILE *pInputFile = fopen(pcInFileName, "r");
    if(!pInputFile)
    {
        perror("\nНе удалось открыть файл для чтения");
        return 1;
    }

    // Открытие файла на запись
    FILE *pOutputFile = fopen(pcOutFileName, "wb");
    if(!pOutputFile)
    {
        perror("\nНе удалось открыть файл для записи\n");
        fclose(pInputFile);
        return 1;
    }
 
    // Буфер под данные
    char *pcBuffer = (char *)malloc(BUFFER_SIZE);
    if(!pcBuffer)
    {
        printf("\nОшибка. Ошибка выделения памяти для буфера\n");
        fclose(pInputFile);
        fclose(pOutputFile);
        remove(pcOutFileName);
        return 1;
    }
    // Количество прочитанных байт
    size_t ulBytesRead;
    // Признак старшего полубайта
    int nHigh = -1;

    while ((ulBytesRead = fread(pcBuffer, 1, BUFFER_SIZE, pInputFile)) > 0) 
    {
        for (size_t i = 0; i < ulBytesRead; i++) 
        {
            int nCurSymbol = pcBuffer[i];

            // Проверка, что текущий символ пробел
            if (isspace(nCurSymbol))
            {
                continue;
            }

             // Проверка, что текущий символ представляет шестнадцатиричный
            if (!isxdigit(nCurSymbol))
            {
                printf("\nОшибка. Некорретные данные в файле\n");
                nResult = 1;
                break;
            }

            // Преобразование текущего HEX символа в Bin
            int nConvertedSymbol = ConvertSymbolHexToBin(nCurSymbol);
            if(nConvertedSymbol == -1)
            {
                printf("\nОшибка. Ошибка конвертации HEX->Bin\n");
                nResult = 1;
                break;
            }

            // Если текущий символ является старшим полубайтом, то сохранение в него значения
            // и переход к конвертации младшего полубайта
            if(nHigh == -1)
            {
                nHigh = nConvertedSymbol;
            }
            else
            {
                // "Сборка" старшего полубайта и младшего
                unsigned char byte = (nHigh << 4) | nConvertedSymbol;

                // Запись преобразованного символа в файл
                if (fwrite(&byte, 1, 1, pOutputFile) != 1) 
                {
                    printf("\nОшибка. Ошибка записи в файл \n");
                    nResult = 1;
                    break;
                }
                
                // Сброс старшего полубайта
                nHigh = -1;
            }
        }

        if (nResult)
        {
            break;
        }
    }

    if(nHigh != -1 && !nResult)
    {
        printf("\nОшибка. Нечетное количество HEX сиволов\n");
        nResult = 1;
    }

    // Освобождение ресурсов
    free(pcBuffer);
    fclose(pInputFile);
    fclose(pOutputFile);

    // В случае ошибки записи в файл, удаление
    if(nResult)
    {
        remove(pcOutFileName);
    }

    return nResult;
}

int main(int argc, char *argv[]) 
{
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif

    // Проверка, если не введены агрументы
    if(argc == 1)
    {
        PrintHelp();
        #ifdef _WIN32
            getchar();
        #endif 

        return 0;
    }

    // Обработка вызова -h
    if(!strcmp(argv[1], "-h"))
    {
        PrintHelp();
        return 0;
    }
    
    // Проверка количества аргументов
    if(argc != 3)
    {
        printf("\nОшибка. Неизвестный параметр.\nИнструкция по использованию -h\n");
        return 1;
    }

    int nResult = 0;
    // Получение имени файла
    char *pcInFileName = argv[2];

    // Проверка, что имя файла не менее 5 символов
    size_t nFileNameLen = strlen(pcInFileName);
    if (nFileNameLen < 5) 
    {
        printf("\nОшибка. Неверное имя файла\n");     
        return 1;
    }   

    // Проверка, что агрумент -a
    if(!strcmp(argv[1], "-a"))
    {
        // Проверка, что имя файла имеет расширение .HEX
        if (strcmp(pcInFileName + nFileNameLen - 4, ".hex")) 
        {
            printf("\nОшибка. Неверный тип файла\n");     
            return 1;
        }

        // Формирование имени выходного файла
        char pcOutFileName[nFileNameLen + 5];
        strcpy(pcOutFileName, pcInFileName);
        strcat(pcOutFileName, ".bin");
        
       nResult = ConvertFileHexToBin(pcInFileName, pcOutFileName);
    }
    // Проверка, что агрумент -b
    else if(!strcmp(argv[1], "-b"))
    {
        // Проверка, что имя файла имеет расширение .bin
        if (strcmp(pcInFileName + nFileNameLen - 4, ".bin")) 
        {
            printf("\nОшибка. Неверный тип файла\n");      
            return 1;
        }

        // Формирование имени выходного файла
        char pcOutFileName[nFileNameLen + 5];
        strcpy(pcOutFileName, pcInFileName);
        strcat(pcOutFileName, ".hex");

        nResult = ConvertFileBinToHex(pcInFileName, pcOutFileName);
    }
    else
    {
        printf("\nОшибка. Неизвестный параметр\nИнструкция по использованию -h\n");     
        return 1;
    }

    if(!nResult)
    {
        printf("\nУспешное конвертирование файла.\n");
    }

    return nResult;
}