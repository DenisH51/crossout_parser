    #ifndef BATTLE_H
    #define BATTLE_H

    #include <stdio.h>
    #include <string.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <windows.h>

    #define NAME_LEN 64
    #define MAP_LEN 64
    #define BATTLE_LEN 64
    #define REASON_LEN 32
    #define STRIPE_LEN 64
    #define WEAPON_LEN 128
    #define WIN_REASON_LEN 64
    #define LINE_LEN 1024
    #define PATH_LEN 256

    #define MAX_PLAYERS 64
    #define MAX_EVENTS 4096
    #define MAX_WEAPONS 16
    #define MAX_BATTLES 50 //creat with malloc 
 
    #define MAX_CRAFTS 16
    #define MAX_MAPS 64

    /*The parser is based on a Battle structure that stores the full match data, including map info, players,
    weapons, and a list of raw log events. Each line of the log is converted into an Event (Damage, Spawn, Score, etc.),
    which is then used to update Player and WeaponStat structures that keep aggregated statistics like damage dealt,
    damage received, and kills.
    */



    //----------------------events------------------------------

    typedef enum {
        EVENT_TYPE_GAMEPLAY_START,
        EVENT_TYPE_SPAWN,
        EVENT_TYPE_PLAYER,
        EVENT_TYPE_DAMAGE,
        EVENT_TYPE_KILL,
        EVENT_TYPE_SCORE,
        EVENT_TYPE_STRIPE,
        EVENT_TYPE_GAMEPLAY_END,
        EVENT_TYPE_UNKNOWN
    } EVENT_TYPE;

    typedef struct {
        char map_name[MAP_LEN];
        char battle_type[BATTLE_LEN];

    } EVENT_GAMEPLAY_START;

    typedef struct {
        int player_id;
        char nickname[NAME_LEN];
        int team;
        int spawnCounter;

    } EVENT_SPAWN;


    typedef struct {
        int player_id;
        int uid;

        char nickname[NAME_LEN];
        int team;
        bool is_bot;

    } EVENT_PLAYER;


    typedef struct {
        char victim[NAME_LEN];
        char attacker[NAME_LEN];
        
        char weapon[WEAPON_LEN];
        double damage;
        bool is_suicide;

    } EVENT_DAMAGE;

    typedef struct {
        char victim[NAME_LEN];
        char killer[NAME_LEN];
        
    } EVENT_KILL;

    typedef struct {
        int player_id;
        char nickname[NAME_LEN];
        int value;
        char reason[REASON_LEN];
    } EVENT_SCORE;

    typedef struct {
        char stripe_name[STRIPE_LEN];
        int player_id;
        char nickname[NAME_LEN];
        int value;

    } EVENT_STRIPE;

    typedef struct {
        char win_reason[WIN_REASON_LEN];
        int winner_team;

        double battle_time;

    } EVENT_GAMEPLAY_END;


    // Event is a temporary container for a parsed log line.
    // Only one type of data is valid at a time (defined by 'type').
    // It is used to process the event and update game state (players, stats),
    typedef struct {
        EVENT_TYPE type;

        union {
            EVENT_GAMEPLAY_START start;
            EVENT_SPAWN spawn;
            EVENT_PLAYER player;
            EVENT_DAMAGE damage;
            EVENT_KILL kill;
            EVENT_SCORE score;
            EVENT_STRIPE stripe;
            EVENT_GAMEPLAY_END end;
        } data;

    } Event;


    //--------------------main structers----------------------


    typedef struct {
        char name[WEAPON_LEN];

        double total_damage;
        int hits;
    } WeaponStat;



    typedef struct {
        int player_id;
        int uid;

        char nickname[NAME_LEN];
        int team;
        int is_bot;

        double damage_dealt;
        double damage_received;

        int kills;
        int deaths;
        int assists;
        int spawn_counter;
        int score;

        WeaponStat weapons[MAX_WEAPONS];
        int weapon_count;

    } Player;



    typedef struct {
        char map_name[MAP_LEN];
        char battle_type[BATTLE_LEN];

        int winner_team;
        double battle_time;
        char winner_reason[REASON_LEN];

        Player players[MAX_PLAYERS];
        int player_count;
    } Battle_record;

    //---------------------global statistic for all battles------------------
    typedef struct {

        char name[WEAPON_LEN];

        int battles;

        int wins;
        int losses;

        double avg_damage;
        double avg_received;

        int avg_score;
        int avg_hits;

    } GlobalCraftStat;

    typedef struct {

        char name[MAP_LEN];

        int battles;

        int wins;
        int losses;

    } GlobalMapStat;

    typedef struct {

        char nickname[NAME_LEN];

        int battles;

        int wins;
        int losses;

        // crafts
        GlobalCraftStat crafts[MAX_CRAFTS];
        int craft_count;

        // maps
        GlobalMapStat maps[MAX_MAPS];
        int map_count;

    } GlobalPlayerStat;







    #endif