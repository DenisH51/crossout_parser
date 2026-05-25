#include "battle.h"
#include "parser.h"

int main(){
    // all battles storage
    int max_battles = MAX_BATTLES;
    
    Battle_record *battle_records = malloc(sizeof(*battle_records) * max_battles);
    if (!battle_records) {
        printf("Memory error: cannot expand battles array\n");
        return -1;
    }

    memset(battle_records, 0, sizeof(*battle_records) * MAX_BATTLES);

    //temorary batle use to update current buttle
    int battle_count = 0;
    Battle_record *current = NULL;


    // create dinamic array for store statistic for all players 
    GlobalStats global_stat = {0};

    global_stat.capacity = MAX_PLAYERS_OVERALL;
    global_stat.player_count = 0;

    global_stat.players = malloc(sizeof(GlobalPlayerStat) * global_stat.capacity);

    if (!global_stat.players) {
        printf("Memory error: cannot expand players array\n");
        return -1;
    }





    //find combat.log 1 automaticly 2 manual

    char path_combat[PATH_LEN];
    FILE *file = NULL;

    while(file == NULL){
        int choice;

        printf("\n1 - auto find log\n");
        printf("2 - manual path\n");
        printf("choose: ");
        scanf("%d", &choice);
        int c;
        while ((c = getchar()) != '\n' && c != EOF);



        


        if(choice == 1){

            //search user profile
            char path_in[PATH_LEN];

            const char *userprofile = getenv("USERPROFILE");

            if(userprofile == NULL){
                printf("Can't get USERPROFILE\n");
                return 1;
            }

            snprintf(path_in, PATH_LEN, "%s\\AppData\\Local\\Targem\\Crossout\\logs", userprofile);




            if(!find_last_folder(path_in, path_combat, PATH_LEN)){
                printf("No folder found\n");
                continue;
            }

            file = fopen(path_combat, "r");

            if (!file) {
                printf("Can't open auto log\n");
            }
        }
        
        else if(choice == 2){

            printf("Enter full path: ");
            fgets(path_combat, PATH_LEN, stdin);
            path_combat[strcspn(path_combat, "\n")] = 0;

            file = fopen(path_combat, "r");

            if (!file) {
                printf("Can't open file\n");
            }
        }

        else {
            printf("Wrong choice\n");
        }

    }

    printf("Open file by path: %s\n", path_combat);
    




    //read by line
    char line[LINE_LEN];

    int empty_reads = 0;

    while (1) {
        if(fgets(line, sizeof(line), file)){
            

            line[strcspn(line, "\n")] = 0;

            Event event = identify_line(line);

            // GAMEPLAY START
            if (event.type == EVENT_TYPE_GAMEPLAY_START) {

                if (current != NULL) {
                    printf("Warning: START without END\n");
                    //save current battle()
                    battle_count++; 
                }
                if (battle_count >= max_battles) {
                    int old_size = max_battles;
                    max_battles *= 2;

                    Battle_record *tmp = realloc(battle_records, sizeof(*battle_records) * max_battles);
                    if (!tmp) {
                        printf("realloc failed\n");
                        free(battle_records);
                        return 1;
                    }

                    battle_records = tmp;

                    memset(&battle_records[old_size], 0, sizeof(*battle_records) * (max_battles - old_size));
                }

                current = &battle_records[battle_count];
                memset(current, 0, sizeof(Battle_record));
                process_event(current, &event);
                
                /*
                //debug
                printf("===== Gameplay '%s' started, map '%s' battle count '%d'======\n", 
                    current->battle_type, 
                    current->map_name, 
                    battle_count);
                */
            }

            // GAMEPLAY END
            else if (event.type == EVENT_TYPE_GAMEPLAY_END) {

                if (current != NULL) {
                    process_event(current, &event);

                    update_global_stats(&global_stat, current);

                    print_global_stats(&global_stat);

                    print_battle(current);

                    battle_count++;
                    current = NULL;
                }
                
            }

            // NORMAL EVENTS
            else {
                if (current != NULL) {
                    process_event(current, &event);
                }
            }

            // -------- DEBUG OUTPUT (optional)
        }

        //wait for update 
        else{
            Sleep(1000);
            clearerr(file);

            empty_reads++;

            if (empty_reads > 50) {
                printf("waiting for log...\n");
                empty_reads = 0;
            }
        }
    }

    fclose(file);

    

    free(battle_records);
    return 0;
}