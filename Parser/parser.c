#include "battle.h"
#include "parser.h"


//function for parse

const char* read_str_to_char(const char *start, char stop, char *group, int max_len_str){
    int i = 0;
    while(*start == ' '){ //clean spaces before the name
        start++;
    }


    while(*start != '\0' && *start != stop && i < max_len_str - 1){//read/write 
        group[i] = *start;
        i++;
        start++;
    }

    while(i > 0 && group[i - 1] == ' '){//clean space behind the name
        i--;
    }
        
    group[i] = '\0';

    if (*start == stop) start++; //increment pointer to poin next char


    return start;
}

const char* read_str_to_pointer(const char *start, const char *stop, char *group, int max_len_str){
    int i = 0;
    while(*start == ' '){ //clean spaces before the name
        start++;
    }


    while(*start != '\0' && start < stop && i < max_len_str - 1){//read/write 
        group[i] = *start;
        i++;
        start++;
    }

    while(i > 0 && group[i - 1] == ' '){//clean space behind the name
        i--;
    }
        
    group[i] = '\0';



    return start;
}

const char* read_int(const char *p, int *group){
    while(*p == ' '){
        p++;
    }
    int value = 0;
    int sign = 1;

    if(*p == '-') {
        sign = -1;
        p++;
    }

    while(*p >= '0' && *p <= '9') {
        value = value * 10 + (*p - '0');
        p++;
    }

    *group = value * sign;

    return p;
}

const char* read_double(const char *p, double *group)
{
    while(*p == ' '){
        p++;
    }
    double value = 0.0;
    double frac = 0.0;
    double divisor = 1.0;
    int sign = 1;

    // minus
    if(*p == '-'){
        sign = -1;
        p++;
    }

    // integer part
    while(*p >= '0' && *p <= '9'){
        value = value * 10.0 + (*p - '0');
        p++;
    }

    // fractional part
    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') {
            frac = frac * 10.0 + (*p - '0');
            divisor *= 10.0;
            p++;
        }
    }

    *group = sign * (value + frac / divisor);

    return p;
}



//function to proccess

void print_weapons(Player *p) {
    if (p->weapon_count == 0) {
        printf("      (no weapons)\n");
        return;
    }

    printf("      Weapons:\n");

    for (int i = 0; i < p->weapon_count; i++) {
        WeaponStat *w = &p->weapons[i];

        printf("        %-10s  dmg: %-6.1f  hits: %d\n",
            w->name,
            w->total_damage,
            w->hits
        );
    }
}

void safe_copy_str(char *dest, const char *src, int size) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

void print_team(Battle_record *battle, const int team){
    printf("----- TEAM %d -----\n", team);
    printf("ID   Name            K   D   DMG     REC     Score   Bot\n");
    printf("--------------------------------------------------------\n");

    for (int i = 0; i < battle->player_count; i++) {
        Player *p = &battle->players[i];

        if (p->team != team)
            continue;

        printf("%-4d %-15s %-3d %-3d %-7.1f %-7.1f %-6d %-3d\n",
            p->player_id,
            p->nickname,
            p->kills,
            p->deaths,
            p->damage_dealt,
            p->damage_received,
            p->score,
            p->is_bot
        );

        print_weapons(p);
    }

    printf("\n");
}

Player* find_player(Battle_record *battle, int player_id){//return adress to found player if no return null
    for (int i = 0; i < battle->player_count; i++) {
        if (battle->players[i].player_id == player_id) {
            return &battle->players[i];
        }
    }

    return NULL;
}

Player* find_player_name(Battle_record *battle, const char target_name[NAME_LEN]){
    for(int i = 0; i < battle->player_count; i++){
        if(strcmp(battle->players[i].nickname, target_name) == 0){
            return &battle->players[i];
        }
    }

    return NULL;
}

WeaponStat* find_weapon(Player *player, const char target_name[WEAPON_LEN]){
    for(int i = 0; i < player->weapon_count; i++){
        if(strcmp(player->weapons[i].name, target_name) == 0){
            return &player->weapons[i];
        }
    }

    return NULL;
}





//function for global stat

GlobalPlayerStat* find_player_global(GlobalStats *global, const char target_name[NAME_LEN]){
    for(int i = 0; i < global->player_count; i++){
        if(strcmp(global->players[i].nickname, target_name) == 0){
            return &global->players[i];
        }

    }

    return NULL;
}

GlobalMapStat* find_map_global(GlobalPlayerStat *players, const char target_map[MAP_LEN]){
    for(int i = 0; i < players->map_count; i++){
        if(strcmp(players->maps[i].name, target_map) == 0){
            return &players->maps[i];
        }

    }

    return NULL;
}




//black list
static const char *RUBBISH_LIST[] = {
    "Ammo",
    "Hover",
    "Engine",
    "Bumper",
    "Generator",
    "Wheel",
    "Track",
    "Bumper",
    "Chassis",
    "Grid",
    "Cabin",
    "PowerGiver",
    "CarPart_Avia",
    "Air_Cushion_01",
    "ModuleAmmo",
    "MechaLeg",
    "Radiator",
    "Radar"
};
static const int RUBBISH_COUNT = sizeof(RUBBISH_LIST) / sizeof(RUBBISH_LIST[0]);

bool is_rubish(const char *name){
    if (!name){
        return true;
    }

    for (int i = 0; i < RUBBISH_COUNT; i++)
    {
        if (strstr(name, RUBBISH_LIST[i]))
        {
            return true;
        }
    }

    return false;
}

//white list
static const char *WEAPON_LIST[] = {
    // guns
    "CarPart_Gun_",

    // artillery / cannons
    "RapidFire_Artillery",
    "BigCannon",
    "Cannon",
    "Mortar",

    // shotguns
    "Shotgun",
    "SawLauncher",

    // machine guns
    "Machinegun",
    "SmartMachinegun",
    "Minigun",

    // rockets / missiles
    "Rocket",
    "Missile",
    "GrenadeLauncher",
    "RocketLauncher",
    "HomingMissile",
    "SurfaceRocket",

    // melee weapons
    "Drill",
    "Chainsaw",
    "Harvester",
    "Tempura",
    "Lacerator",

    // energy / special
    "Laser",
    "Plasma",
    "Emitter",
    "Destructor",
    "Scorpion",
    "Kaiju",
    "Assembler",
    "TrainWheel_Launcher",

    // flame / electricity
    "Flamethrower",
    "Spark",
    "Tesla",

    // drones / turrets
    "Drone",
    "Turret",

    // crossbows
    "Crossbow",
    "SniperCrossbow",

    "WheelRocket"
};
static const int WEAPON_COUNT = sizeof(WEAPON_LIST) / sizeof(WEAPON_LIST[0]);


bool is_weapon(const char *name){
    if (!name){
        return true;
    }

    for (int i = 0; i < WEAPON_COUNT; i++)
    {
        if (strstr(name, WEAPON_LIST[i]))
        {
            return true;
        }
    }

    return false;
}

void sort_wepons_in_craft(GlobalCraftStat *craft){
    for(int i = 0; i < craft->weapons_count - 1; i++){

        for(int j = i + 1; j < craft->weapons_count; j++){

            if(strcmp(craft->weapons[i].name, craft->weapons[j].name) > 0){

                //swap
                GlobalWeaponStat tmp = craft->weapons[i];
                craft->weapons[i] = craft->weapons[j];
                craft->weapons[j] = tmp;
            }
        }
    }
}

GlobalWeaponStat* find_global_weapon(GlobalCraftStat *temp_craft, const char target[NAME_LEN], int max){
    for(int i = 0; i < max; i++){
        if(strcmp(temp_craft->weapons[i].name, target) == 0){
            return &temp_craft->weapons[i];
        }
    }

    return NULL;
}

void create_craft_global(GlobalCraftStat *temp_craft, const Player *player){
    if (!temp_craft || !player){
        return;
    }
        
    memset(temp_craft, 0, sizeof(*temp_craft));

    temp_craft->weapons_count = 0;


    //make a list with gun
    for(int i = 0; i < player->weapon_count; i++){
        //check weapon or not incomprehensible thing
        bool should_add = false;

        if (is_weapon(player->weapons[i].name)){
            should_add = true;
        }
        /*else if (!is_rubish(player->weapons[i].name) && player->weapons[i].hits > 1){
            should_add = true;
        }*/


        //if weapon add to list
        if(should_add){
            if(temp_craft->weapons_count >= MAX_WEAPONS){
                break;
            }

            GlobalWeaponStat *found_weapon = find_global_weapon(temp_craft, player->weapons[i].name, temp_craft->weapons_count);

            //weapon not found
            if(found_weapon == NULL){
                //create weapon in player
                found_weapon = &temp_craft->weapons[temp_craft->weapons_count];
                memset(found_weapon, 0, sizeof(*found_weapon));
                safe_copy_str(found_weapon->name, player->weapons[i].name, WEAPON_LEN);
                temp_craft->weapons_count++;
                
            }

            //update weapon data
            found_weapon->total_damage += player->weapons[i].total_damage;
            found_weapon->hits += player->weapons[i].hits;
            
        }
    }
    //buble sort
    sort_wepons_in_craft(temp_craft);
}




//-----------main process function-----------------------

void process_damage(Battle_record *battle, Event *event){
    //for attaker
    Player *attacker = find_player_name(battle, event->data.damage.attacker);
    

    //if found player
    if(attacker != NULL){
        WeaponStat *w = find_weapon(attacker, event->data.damage.weapon);
        if(w != NULL){   //if find wepon
            w->hits++;
            w->total_damage += event->data.damage.damage;
            attacker->damage_dealt += event->data.damage.damage;
        }
        else{ //if not find weapon -> create weapon in attaker
            if (attacker->weapon_count < MAX_WEAPONS) {

                w = &attacker->weapons[attacker->weapon_count];

                safe_copy_str(w->name, event->data.damage.weapon, WEAPON_LEN);

                w->hits = 1;
                w->total_damage = event->data.damage.damage;
                attacker->damage_dealt += event->data.damage.damage;

                attacker->weapon_count++;
            }
            else {
                printf("Max weapons reached for %s\n", attacker->nickname);
            }
        }
    }
    else{
        printf("Damage: attacker not found\n");
    }

    //for victim
    Player *victim  = find_player_name(battle, event->data.damage.victim);
    
    if(victim != NULL){
        victim->damage_received += event->data.damage.damage;
    }

    /*
    //debug
    printf("Damage '%s' dealt '%.1f' victim '%s' received '%.1f'\n",
        attacker ? attacker->nickname : "NULL",
        attacker ? attacker->damage_dealt : 0.0,
        victim ? victim->nickname : "NULL",
        victim ? victim->damage_received : 0.0
    );

    */


}

void process_spawn(Battle_record *battle, Event *event){
    Player *p = find_player(battle, event->data.spawn.player_id);

    if (p != NULL) {
        p->spawn_counter = event->data.spawn.spawnCounter;
        p->team = event->data.spawn.team;
        return;
    }

    if (battle->player_count >= MAX_PLAYERS) {
        printf("Max players filled\n");
        return;
    }

    p = &battle->players[battle->player_count];

    p->player_id = event->data.spawn.player_id;
    p->spawn_counter = event->data.spawn.spawnCounter;
    p->team = event->data.spawn.team;

    safe_copy_str(p->nickname, event->data.spawn.nickname, NAME_LEN);
    
    /*
    printf("spawn '%s' id '%d' spawn '%d' team '%d\n", 
        battle->players[battle->player_count].nickname, 
        battle->players[battle->player_count].player_id,
        battle->players[battle->player_count].spawn_counter,
        battle->players[battle->player_count].team);
    */


    battle->player_count++;


    
}

void process_player(Battle_record *battle, Event *event){

    Player *p = find_player(battle, event->data.player.player_id);

    //if find player
    if (p != NULL) {
        safe_copy_str(p->nickname, event->data.player.nickname, NAME_LEN);
        p->team = event->data.player.team;
        p->is_bot = event->data.player.is_bot;
        p->uid = event->data.player.uid;
        return;
    }

    if (battle->player_count >= MAX_PLAYERS) {
        printf("Max players filled\n");
        return;
    }

    //create new player and write there data 
    p = &battle->players[battle->player_count];

    p->player_id = event->data.player.player_id;
    p->uid = event->data.player.uid;

    safe_copy_str(p->nickname, event->data.player.nickname, NAME_LEN);

    p->team = event->data.player.team;
    p->is_bot = event->data.player.is_bot;


    battle->player_count++;

}

void process_kill(Battle_record *battle, Event *event){
    const char *victim = event->data.kill.victim;
    const char *killer = event->data.kill.killer;

    // is suicide
    if (strcmp(victim, killer) == 0){
        Player *p = find_player_name(battle, victim);
        if (p != NULL){
            p->deaths++;
        }
    }
    else{
        Player *v = find_player_name(battle, victim);
        if (v != NULL){
            v->deaths++;
        }

        Player *k = find_player_name(battle, killer);
        if (k != NULL){
            k->kills++;
        }
    }
}

void process_score(Battle_record *battle, Event *event){
    Player *p = find_player(battle, event->data.score.player_id);

    if (!p) {
        printf("Score: unknown player %d\n", event->data.score.player_id);
        return;
    }

    p->score += event->data.score.value;
}

void update_global_stats(GlobalStats *global, const Battle_record *battle){

    for(int i = 0; i < battle->player_count; i++){
        //find record by nickname
        const char *target_name = battle->players[i].nickname;

        GlobalPlayerStat *found_player = find_player_global(global, target_name);
        

        //if player not found -> create player
        if(found_player == NULL){
            //try to expand array
            if(global->player_count >= global->capacity){
                int new_capacity = global->capacity * 2;

                GlobalPlayerStat *tmp =realloc(global->players, sizeof(GlobalPlayerStat) * new_capacity);

                if (tmp == NULL) {
                    printf("Failed to expand players array\n");
                    return;
                }

                global->players = tmp;
                global->capacity = new_capacity;
            }
            
            //create new player
            found_player = &global->players[global->player_count];
            memset(found_player, 0, sizeof(*found_player));

            safe_copy_str(found_player->nickname, battle->players[i].nickname, NAME_LEN);

            global->player_count++;
        }
            
        //update data for player
        found_player->battles++;

        if(battle->winner_team == battle->players[i].team){
            found_player->wins++;
        }
        else{
            found_player->losses++;
        }
    

        found_player->score += battle->players[i].score;

        found_player->total_damage += battle->players[i].damage_dealt;
        found_player->total_received += battle->players[i].damage_received;

        found_player->total_kill += battle->players[i].kills;
        found_player->total_death += battle->players[i].deaths;

        



        //find map by name------------------------------------
        const char *target_map = battle->map_name;

        GlobalMapStat *found_map = find_map_global(found_player, target_map);
        
        //if found map
        if(found_map != NULL){
            //update data
            found_map->battles++;
            
            if(battle->players[i].team == battle->winner_team){
                found_map->wins++;
            }
            else{
                found_map->losses++;
            }
        }
        //create new map
        else{
            if(found_player->map_count >= MAX_MAPS){
                continue;
            }
            GlobalMapStat *new_map = &found_player->maps[found_player->map_count];

            memset(new_map, 0, sizeof(*new_map));

            safe_copy_str(new_map->name, battle->map_name, MAP_LEN - 1);

            new_map->battles = 1;

            if (battle->winner_team == battle->players[i].team) {
                new_map->wins = 1;
            }
            else {
                new_map->losses = 1;
            }

            found_player->map_count++;
            


            //find craft by name----------------------------------------------
            //create set of weapons by cheking is gun, created set inside sort A-Z 
            GlobalCraftStat temp_craft = {0};
            create_craft_global(&temp_craft, &battle->players[i]);

            printf("name: '%s'  \n", target_name);
            for(int i = 0; i < temp_craft.weapons_count; i++){
                printf("         '%s'           '%9f'       '%d'\n",
                    temp_craft.weapons[i].name, 
                    temp_craft.weapons[i].total_damage,
                    temp_craft.weapons[i].hits);
            }
            printf("\n");

            //find neccesery craft 
            //GlobalCraftStat *found_craft = find_craft_global(found_player, temp_craft);

            //if craft not found -> create 



            //update data 
        }
    }
}





//-------------------------------------------------------------------------------------------------------------------------------


//take 1 line read and identify which event is this and back name event to main 
Event identify_line(const char *line){
    Event event;
    memset(&event, 0, sizeof(Event));

    // 1. DAMAGE
    if (strstr(line, "Damage.")) {
        event.type = EVENT_TYPE_DAMAGE;
        const char *p = line;

        // victim
        p = strstr(p, "Victim:"); //return pointer where is found string 
        if (!p) return event;
        p += strlen("Victim:");

        p = read_str_to_char(p, ',', event.data.damage.victim, NAME_LEN);


        // attacker
        p = strstr(p, "attacker:");
        if (!p) return event;
        p += strlen("attacker:");

        p = read_str_to_char(p, ',',event.data.damage.attacker, NAME_LEN);


        // weapon
        p = strstr(p, "weapon '");
        if (!p) return event;
        p += strlen("weapon '");

        p = read_str_to_char(p, '\'', event.data.damage.weapon, WEAPON_LEN);

        // damage value
        p = strstr(p, "damage:");
        if (!p) return event;
        p += strlen("damage:");

        p = read_double(p, &event.data.damage.damage);
        

        //is_suicide
        event.data.damage.is_suicide = (strstr(line, "SUICIDE") != NULL);

        return event;
    }

    
    // 2. KILL
    if (strstr(line, "Kill.")) {
        event.type = EVENT_TYPE_KILL;

        // victim
        const char *v_start = strstr(line, "Victim:"); //return pointer where is found string 
        if (!v_start) return event;
        v_start += strlen("Victim:");

        //killer
        const char *k_label = strstr(line, "killer:");
        if (!k_label) return event;

        const char *k_start = k_label + strlen("killer:");

        //----------------------------------------------


        read_str_to_pointer(v_start, k_label, event.data.kill.victim, NAME_LEN);
        read_str_to_char(k_start, '\0', event.data.kill.killer, NAME_LEN);
    

        return event;
    }
    
    // 3. SCORE
    if (strstr(line, "Score")) {
        event.type = EVENT_TYPE_SCORE;
        const char *p = line;

        //Player id
        p = strstr(p, "player:");
        if(!p) return event;
        p += strlen("player:");


        p = read_int(p, &event.data.score.player_id);

        //nick
        p = strstr(p, "nick:");
        if (!p) return event;
        p += strlen("nick:");

        p = read_str_to_char(p, ',', event.data.score.nickname, NAME_LEN);
        
        //score
        p = strstr(line, "Got:");
        if(!p) return event;
        p += strlen("Got:");

        p = read_int(p, &event.data.score.value);
        

        //reason
        p = strstr(line, "reason:");
        if(!p) return event;
        p += strlen("reason:");

        p = read_str_to_char(p, '\0', event.data.score.reason, REASON_LEN);

        return event;
    }

    
    // 4. SPAWN
    if(strstr(line, "Spawn player")){
        event.type = EVENT_TYPE_SPAWN;
        const char *p = line;

        //player id
        p = strstr(p, "Spawn player");
        if(!p) return event;
        p += strlen("Spawn player");

        p = read_int(p, &event.data.spawn.player_id);

        //nick
        p = strchr(line, '[');
        if(!p) return event;
        p++;

        p = read_str_to_char(p, ']', event.data.spawn.nickname, NAME_LEN);

        //team
        p = strstr(p, "team");
        if(!p) return event;
        p += strlen("team");

        p = read_int(p, &event.data.spawn.team);


        //spawnCounter
        p = strstr(p, "spawnCounter");
        if(!p) return event;
        p += strlen("spawnCounter");

        p = read_int(p, &event.data.spawn.spawnCounter);

        return event;
    }



    // 5. STRIPE
    if(strstr(line, "Stripe '")){
        event.type = EVENT_TYPE_STRIPE;
        const char *p = line;

        //stripe name
        p = strstr(p, "Stripe '");
        if(!p) return event;
        p += strlen("Stripe '");

        p = read_str_to_char(p, '\'', event.data.stripe.stripe_name, STRIPE_LEN);

        //value
        p = strstr(p, "increased by");
        if(!p) return event;
        p += strlen("increased by");

        p = read_int(p, &event.data.stripe.value);

        //player id
        p = strstr(p, "for player");
        if(!p) return event;
        p += strlen("for player");

        p = read_int(p, &event.data.stripe.player_id);

        //nick
        p = strchr(p, '[');
        if(!p) return event;
        p++;

        p = read_str_to_char(p, ']', event.data.stripe.nickname, NAME_LEN);

        return event;
    }


    // 6. PLAYER
    if(strstr(line, "player ") && strstr(line, "uid")){
        event.type = EVENT_TYPE_PLAYER;
        const char *p = line;

        //player id
        p = strstr(p, "player");
        if(!p) return event;
        p += strlen("player");

        p = read_int(p, &event.data.player.player_id);

        //uid
        p = strstr(p, "uid");
        if(!p) return event;
        p += strlen("uid");

        p = read_int(p, &event.data.player.uid);

        //nickname
        p = strstr(p, "nickname:");
        if(!p) return event;
        p += strlen("nickname:");

        p = read_str_to_char(p, ',', event.data.player.nickname, NAME_LEN);

        //team
        p = strstr(p, "team:");
        if(!p) return event;
        p += strlen("team:");

        p = read_int(p, &event.data.player.team);

        //is bot
        p = strstr(p, "bot:");
        if(!p) return event;
        p += strlen("bot:");

        int temp;
        p = read_int(p, &temp);
        event.data.player.is_bot = (temp != 0);

        return event;

    }

    // 7. GAMEPLAY START
    if(strstr(line, "Gameplay '")){
        event.type = EVENT_TYPE_GAMEPLAY_START;
        const char *p = line;


        //battle type
        p = strstr(p, "Gameplay '");
        if(!p) return event;
        p += strlen("Gameplay '");

        p = read_str_to_char(p, '\'', event.data.start.battle_type, BATTLE_LEN);

        //map
        p = strstr(p, ", map '");
        if(!p) return event;
        p += strlen(", map '");

        p = read_str_to_char(p, '\'', event.data.start.map_name, MAP_LEN);
        

        return event;

    }

    // 8. GAMEPLAY FINISH
    if(strstr(line, "Gameplay finish")){
        event.type = EVENT_TYPE_GAMEPLAY_END;
        const char *p = line;


        //winer team
        p = strstr(p, "winner team");
        if(!p) return event;
        p += strlen("winner team");

        p = read_int(p, &event.data.end.winner_team);

        //win reason
        p = strstr(p, "win reason:");
        if(!p) return event;
        p += strlen("win reason:");

        p = read_str_to_char(p, ',', event.data.end.win_reason, REASON_LEN);

        //time 
        p = strstr(p, "battle time:");
        if(!p) return event;
        p += strlen("battle time:");

        p = read_double(p, &event.data.end.battle_time);

        return event;
    }
    // 9. UNKNOWN
    event.type = EVENT_TYPE_UNKNOWN;
    return event;
    
}

//take event that gave function parse_line() and updates BattleRecord
void process_event(Battle_record *battle, Event *event){
    switch (event->type) {

    case EVENT_TYPE_DAMAGE:
        process_damage(battle, event);
        break;

    case EVENT_TYPE_KILL:
        process_kill(battle, event);
        break;

    case EVENT_TYPE_SCORE:
        process_score(battle, event);

        break;

    case EVENT_TYPE_SPAWN:
        process_spawn(battle, event);

        break;

    case EVENT_TYPE_STRIPE:
        // обработка стриков
        // event->data.stripe
        break;

    case EVENT_TYPE_PLAYER:
        process_player(battle, event);

        break;

    case EVENT_TYPE_GAMEPLAY_START:
        safe_copy_str(battle->battle_type, event->data.start.battle_type, BATTLE_LEN);
        safe_copy_str(battle->map_name, event->data.start.map_name, MAP_LEN);

        break;

    case EVENT_TYPE_GAMEPLAY_END:
        battle->winner_team = event->data.end.winner_team;
        battle->battle_time = event->data.end.battle_time;
        safe_copy_str(battle->winner_reason, event->data.end.win_reason, REASON_LEN);


        /*
        //debug-----------------------------------------------------------
        printf("winner team '%d' battle time '%0.1f' reason '%s'\n", 
            battle->winner_team, 
            battle->battle_time, 
            battle->winner_reason);

        for(int i = 0; i < battle->player_count; i++){
            printf("player %d uid %d nick %s team %d bot %d score %d\n",
            battle->players[i].player_id,
            battle->players[i].uid,
            battle->players[i].nickname,
            battle->players[i].team,
            battle->players[i].is_bot,
            battle->players[i].score);
        }
        

        for(int i = 0; i < battle->player_count; i++){
            printf("'%s' kills '%d' death '%d'\n", 
                battle->players[i].nickname,
                battle->players[i].kills,
                battle->players[i].deaths);
        }
        */

        
        break;

    default:
        break;
    }
}


void print_battle(Battle_record *b) {
    printf("\n");
    printf("==================================================\n");
    printf("Map: %s | Type: %s\n", b->map_name, b->battle_type);
    printf("Time: %.1f sec | Winner: Team %d (%s)\n",
           b->battle_time,
           b->winner_team,
           b->winner_reason);
    printf("==================================================\n\n");

    for (int team = 1; team <= 2; team++) {

        printf("TEAM %d %s\n",
               team,
               (b->winner_team == team ? "(WINNER)" : ""));
        printf("--------------------------------------------------\n");

        

        for (int i = 0; i < b->player_count; i++) {
            Player *p = &b->players[i];

            if (p->team != team) continue;
            printf("ID   Name               K   D   DMG       REC       Score   Bot\n");
            printf("-----------------------------------------------------------------\n");
            printf("%-4d %-18s %-3d %-3d %-9.1f %-9.1f %-6d %-3d\n",
                   p->player_id,
                   p->nickname,
                   p->kills,
                   p->deaths,
                   p->damage_dealt,
                   p->damage_received,
                   p->score,
                   p->is_bot);

            if (p->weapon_count > 0) {
                printf("      Weapons:\n");

                for (int w = 0; w < p->weapon_count; w++) {
                    WeaponStat *ws = &p->weapons[w];

                    printf("        %-35s dmg: %-8.1f hits: %d\n",
                           ws->name,
                           ws->total_damage,
                           ws->hits);
                }
            } else {
                printf("      (no weapons)\n");
            }

            printf("\n");
        }

        printf("\n");
    }
}

int find_last_folder(const char *base_path, char *out_path, int max_len) {
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", base_path);

    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA(search_path, &data);

    if (hFind == INVALID_HANDLE_VALUE) {
        return 0;
    }

    char latest[MAX_PATH] = "";

    do {
        if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0)
            continue;

        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

            // сравнение по имени (у тебя формат даты → это работает)
            if (strcmp(data.cFileName, latest) > 0) {
                strcpy(latest, data.cFileName);
            }
        }

    } while (FindNextFileA(hFind, &data));

    FindClose(hFind);

    if (latest[0] == '\0') {
        return 0;
    }

    snprintf(out_path, max_len, "%s\\%s\\combat.log", base_path, latest);

    return 1;
}
