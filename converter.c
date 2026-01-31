#include <stdio.h>
#include <string.h>

// Функция конвертации HEX файла в бинарный
// [in] pcInFileName    имя входнохо файла
// [in] pcOutFileName   имя выходного файла
// result               результат выполнения
int ConvertHexToBin(const char *pcInFileName, const char *pcOutFileName) 
{
    // Открываем на чтение файл
    FILE *pInputFile = fopen(pcInFileName, "r");
    if(!pInputFile)
    {
        printf("\nНе удалось открыть файл для чтения\n");
        return 1;
    }

    // Открываем файл на запись
    FILE *pOutputFile = fopen(pcOutFileName, "wb");
    if(!pOutputFile)
    {
        printf("\nНе удалось открыть файл для записи\n");
        fclose(pInputFile);
        return 1;
    }

    fclose(pInputFile);
    fclose(pOutputFile);

    return 0;
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
            printf("\nНеверное имя файла.\n");
            return 1;
        }   

        // Проверка, что имя файла имеет расширение .hex
        if (strcmp(pcInFileName + nFileNameLen - 4, ".hex")) 
        {
            printf("\nНеверное имя файла.\n");
            return 1;
        }

        // Формирование имени выходного файла
        char pcOutFileName[nFileNameLen + 5];
        strcpy(pcOutFileName, pcInFileName);
        strcat(pcOutFileName, ".bin");
        

       nResult = ConvertHexToBin(pcInFileName, pcOutFileName);
    }

    printf("\nEnd Project\n");
   
    return nResult;
}