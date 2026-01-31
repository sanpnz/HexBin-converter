#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

// Функция конвертации Hex символа в бинарный
// [in] cSymbol    конвертируемый Hex символ
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

// Функция конвертации HEX файла в бинарный
// [in] pcInFileName    имя входнохо файла
// [in] pcOutFileName   имя выходного файла
// result               результат выполнения
int ConvertFileHexToBin(const char *pcInFileName, const char *pcOutFileName) 
{
    int nResult = 0;

    // Открытие файла на чтение
    FILE *pInputFile = fopen(pcInFileName, "r");
    if(!pInputFile)
    {
        printf("\nНе удалось открыть файл для чтения\n");
        return 1;
    }

    // Открытие файла на запись
    FILE *pOutputFile = fopen(pcOutFileName, "wb");
    if(!pOutputFile)
    {
        printf("\nНе удалось открыть файл для записи\n");
        fclose(pInputFile);
        return 1;
    }
 
    // Буфер под данные
    char pcBuffer[4096];
    // Количество прочитанных байт
    size_t ulBytesRead;
    // Признак старшего полубайта
    int nHigh = -1;

    while ((ulBytesRead = fread(pcBuffer, 1, sizeof(pcBuffer), pInputFile)) > 0) 
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

            // Преобразование текущего Hex символа в Bin
            int nConvertedSymbol = ConvertSymbolHexToBin(nCurSymbol);
            if(nConvertedSymbol == -1)
            {
                printf("\nОшибка. Ошибка конвертации Hex->Bin\n");
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
                    printf("\nОшибка. Ошибка записи в файл\n");
                    nResult = 1;
                    break;
                }
                
                // Сброс старшего полубайта
                nHigh = -1;
            }
        }

        if (nResult)
        {
            printf("\nОшибка\n");
            break;
        }
    }
    
    fclose(pInputFile);
    fclose(pOutputFile);

    return nResult;
}

int main(int argc, char *argv[]) 
{
    // Обработка вызова -h
    if(argc == 2 && !strcmp(argv[1], "-h"))
    {
        printf("\nRequested help\n");
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

    // Проверка, что агрумент -a
    if(!strcmp(argv[1], "-a"))
    {
        // Проверка, что имя файла не менее 5 символов
        size_t nFileNameLen = strlen(pcInFileName);
        if (nFileNameLen < 5) 
        {
            printf("\nОшибка. Неверное имя файла\n");
            return 1;
        }   

        // Проверка, что имя файла имеет расширение .hex
        if (strcmp(pcInFileName + nFileNameLen - 4, ".hex")) 
        {
            printf("\nОшибка.Неверное имя файла\n");
            return 1;
        }

        // Формирование имени выходного файла
        char pcOutFileName[nFileNameLen + 5];
        strcpy(pcOutFileName, pcInFileName);
        strcat(pcOutFileName, ".bin");
        

       nResult = ConvertFileHexToBin(pcInFileName, pcOutFileName);
    }

    printf("\nEnd Project\n");
   
    return nResult;
}