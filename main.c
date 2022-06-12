#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"
#include "leak_detector_c.c"

typedef struct monster
{
    char *m_name; // declaring monster name
    char *st_element; // declaring the elements
    int m_population; // Population
} monster;

typedef struct region // Declaring the regions
{
    char *m_name;  // monster  name
    int nmonsters; // number of monsters 
    int total_population; // total population of monters 
    monster **monsters;
} region;

typedef struct itinerary 
{
    int nregions;
    region **regions;
    int captures;
} itinerary;

typedef struct trainer // small monster trainer
{
    char *m_name;
    itinerary *visits;
} trainer;

monster *monsterCreation(char *m_name, char *st_element, int m_population) // Function for creating the monsters which includes the monster population
{
    monster *monster = malloc(sizeof(monster));
    monster->m_name = malloc((strlen(m_name) + 1) * sizeof(char));
    monster->st_element = malloc((strlen(st_element) + 1) * sizeof(char));
    strcpy(monster->m_name, m_name);
    strcpy(monster->st_element, st_element);
    monster->m_population = m_population;

    return monster;
}

monster **readingMonsters(FILE *infile, int *monsterCount) // The function that reads the monsters from the input file
{
    char m_name[50];
    char st_element[50];
    int m_population;
    monster **monsterList = malloc(sizeof(monster) * *monsterCount);

    for (int i = 0; i < *monsterCount; i++)
    {
        fscanf(infile, "%s %s %d", m_name, st_element, &m_population);
        monsterList[i] = monsterCreation(m_name, st_element, m_population);
        printf("%s %s %d\n", monsterList[i]->m_name, monsterList[i]->st_element, monsterList[i]->m_population);
    }
    return monsterList;
}

region *regionCreation(FILE *infile, char *m_name, int nmonsters, int monsterCount, monster **monsters) // function for creating the region 
// of the monters -> this is read from the file
{
    char monsterName[50]; // monster name declaration
    region *region = malloc(sizeof(region)); // find the size of the region
    region->m_name = malloc((strlen(m_name) + 1) * sizeof(char));
    strcpy(region->m_name, m_name);
    region->monsters = malloc(nmonsters * sizeof(monster));
    region->nmonsters = nmonsters;

    printf("%s\n", region->m_name); // print the region per monster name
    printf("%d monsters\n", nmonsters); // print the number of monsters
    int totalPop = 0;
    for (int i = 0; i < nmonsters; i++)
    {
        fscanf(infile, "%s", monsterName); // find the standard input of from the file
        for (int j = 0; j < monsterCount; j++)
        {
            if (strcmp(monsters[j]->m_name, monsterName) == 0)
            {
                region->monsters[i] = monsters[j];
                totalPop += monsters[j]->m_population;
                printf("%s\n", region->monsters[i]->m_name);
            }
        }
    }

    region->total_population = totalPop;
 
    return region; // returns the region
}

region **readingRegions(FILE *infile, int *countRegions, monster **monsterList, int monsterCount) //function for reading the regions
// done from the file where the region is counted
// then the monster list will be populated
{
    char temp[50];
    char m_name[50];
    int mcount;
    region **regionList = malloc(sizeof(region) * *countRegions); //find the size of the region list

    for (int i = 0; i < *countRegions; i++)
    {
        fscanf(infile, "%s", m_name);
        fscanf(infile, "%d %s", &mcount, temp);

        regionList[i] = regionCreation(infile, m_name, mcount, monsterCount, monsterList); // call the region that was created 

        printf("\n\n");
    }

    return regionList; // returns the regions list
}

trainer *readingTrainers(FILE *infile, int *trainerCount, region **regionList, int countRegions) // reading trainers function
// from the input file
// trainer is counted from the region list
{
    char temp[50];
    char m_name[50];
    char regionName[50];
    int captures;
    int regions;

    trainer *trainerList = malloc(*trainerCount * sizeof(trainer)); // size of the trainer based on trainerCount

    for (int i = 0; i < *trainerCount; i++)
    {
        fscanf(infile, "%s", m_name);
        trainerList[i].m_name = malloc((strlen(m_name) + 1) * sizeof(char));
        strcpy(trainerList[i].m_name, m_name);

        fscanf(infile, "%d %s", &captures, temp);
        fscanf(infile, "%d %s", &regions, temp);

        trainerList[i].visits = malloc(sizeof(itinerary));
        trainerList[i].visits->nregions = regions;
        trainerList[i].visits->captures = captures;

        trainerList[i].visits->regions = malloc(regions * sizeof(region));

        printf("%s\n", trainerList[i].m_name);
        printf("%d captures\n", trainerList[i].visits->captures); // prints the number of captures 
        printf("%d regions\n", regions); // prints the regions

        for (int j = 0; j < regions; j++)
        {
            fscanf(infile, "%s", regionName);
            for (int k = 0; k < countRegions; k++)
            {
                if (strcmp(regionList[k]->m_name, regionName) == 0)
                {
                    trainerList[i].visits->regions[j] = regionList[k];
                    printf("%s\n", trainerList[i].visits->regions[j]->m_name);
                }
            }
        }
        printf("\n");
    }
    return trainerList;  // returns the trainer list or list of trainer
}

void processing_of_inputs(FILE *ofp, monster **monsterList, int monsterCount, region **regionList, int regionCount, trainer *trainerList, int trainerCount)
// function of processing the input from the file
{
    for (int i = 0; i < trainerCount; i++)
    {
        fprintf(ofp, "%s\n", trainerList[i].m_name);
        for (int j = 0; j < trainerList[i].visits->nregions; j++)
        {
            int mPopulation;
            fprintf(ofp, "%s\n", trainerList[i].visits->regions[j]->m_name);
            for (int k = 0; k < trainerList[i].visits->regions[j]->nmonsters; k++)
            {
                int tPopulation = trainerList[i].visits->regions[j]->monsters[k]->m_population;
                int captures = trainerList[i].visits->captures;
                float mcount = (double)tPopulation / trainerList[i].visits->regions[j]->total_population;
                mcount *= captures;
                int caught = round(mcount);
                if (caught != 0)
                {
                    fprintf(ofp, "%d %s\n", caught, trainerList[i].visits->regions[j]->monsters[k]->m_name);
                }
                printf("%d %d %f\n", tPopulation, trainerList[i].visits->regions[j]->total_population, mcount);
            }
            mPopulation = 0;
        }
        if (i < trainerCount - 1)
            fprintf(ofp, "\n");
    }
}

void releasing_of_memory(monster **monsterList, int monsterCount, region **regionList, int regionCount, trainer *trainerList, int trainerCount)
// function for realising the memory 
{
    for (int i = 0; i < monsterCount; i++)
    {
        free(monsterList[i]->m_name);
        free(monsterList[i]->st_element);
        free(monsterList[i]);
    }

    free(monsterList);

    for (int i = 0; i < regionCount; i++)
    {
        free(regionList[i]->m_name);
        free(regionList[i]->monsters);
        free(regionList[i]);
    }

    free(regionList);

    for (int i = 0; i < trainerCount; i++)
    {
        free(trainerList[i].m_name);
        free(trainerList[i].visits->regions);
        free(trainerList[i].visits);
    }
    free(trainerList); // free the lists
}

int main(void)  // The driver function
{
    atexit(report_mem_leak); //detection of memory leak
    FILE *inFile, *outFile;

    inFile = fopen("as1-sample-input_1.txt", "r"); // reading the input file 

    if (inFile != NULL)
    {
        char temp[50];
        int monsterCount;
        int regionCount;
        int trainerCount;

        fscanf(inFile, "%d %s", &monsterCount, temp);

        printf("%d Monsters\n", monsterCount);

        monster **monsterList = readingMonsters(inFile, &monsterCount);

        fscanf(inFile, "%d %s", &regionCount, temp);

        printf("\n%d Regions\n\n", regionCount);

        region **regionList = readingRegions(inFile, &regionCount, monsterList, monsterCount);

        fscanf(inFile, "%d %s", &trainerCount, temp);

        printf("%d Trainers\n", trainerCount);

        trainer *trainerList = readingTrainers(inFile, &trainerCount, regionList, regionCount);

        outFile = fopen("as1-sample-output.txt", "w"); // writing the output file 

        processing_of_inputs(outFile, monsterList, monsterCount, regionList, regionCount, trainerList, trainerCount);

        releasing_of_memory(monsterList, monsterCount, regionList, regionCount, trainerList, trainerCount);

        fclose(inFile);
        fclose(outFile);
    }
    else
    {
        printf("Kindly provide correct input file");
        exit(-1);
    }

    return 0;
}
