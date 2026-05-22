#include "battle.h"
#include <string.h>


//take 1 line read and identify which event is this and back name event to main 
Event identify_line(const char *line);

//take event that gave function parse_line() and updates BattleRecord
void process_event(Battle_record *battle, Event *event);

void print_battle(Battle_record *battle);

int find_last_folder(const char *base_path, char *out_path, int max_len);

void update_global_stats(GlobalStats *global, const Battle_record *battle);