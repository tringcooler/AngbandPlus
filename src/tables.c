/* File: tables.c */

/* Purpose: Angband Tables */

/*
* Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
*
* This software may be copied and distributed for educational, research, and
* not for profit purposes provided that this copyright and statement are
* included in all such copies.
*/

#include "angband.h"

/*
* Global array for looping through the "keypad directions"
*/
s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
* Global arrays for converting "keypad direction" into offsets
*/
s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
* Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
*/
s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };



/*
* Global array for converting numbers to uppercase hecidecimal digit
* This array can also be used to convert a number to an octal digit
*/
char hexsym[16] =
{
'0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


/*
* Stat Table (INT/WIS) -- Number of half-spells per level
*/
byte adj_mag_study[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	2       /* 12 */,
	2       /* 13 */,
	2       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	2       /* 18/00-18/09 */,
	2       /* 18/10-18/19 */,
	2       /* 18/20-18/29 */,
	2       /* 18/30-18/39 */,
	2       /* 18/40-18/49 */,
	3       /* 18/50-18/59 */,
	3       /* 18/60-18/69 */,
	3       /* 18/70-18/79 */,
	3       /* 18/80-18/89 */,
	4       /* 18/90-18/99 */,
	4       /* 18/100-18/109 */,
	4       /* 18/110-18/119 */,
	5       /* 18/120-18/129 */,
	5       /* 18/130-18/139 */,
	5       /* 18/140-18/149 */,
	5       /* 18/150-18/159 */,
	5       /* 18/160-18/169 */,
	5       /* 18/170-18/179 */,
	5       /* 18/180-18/189 */,
	5       /* 18/190-18/199 */,
	5       /* 18/200-18/209 */,
	6       /* 18/210-18/219 */,
	6       /* 18/220+ */
};


/*
* Stat Table (INT/WIS) -- extra half-mana-points per level
*/
byte adj_mag_mana[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	2       /* 9 */,
	2       /* 10 */,
	2       /* 11 */,
	2       /* 12 */,
	2       /* 13 */,
	2       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	16      /* 18/190-18/199 */,
	17      /* 18/200-18/209 */,
	17      /* 18/210-18/219 */,
	18      /* 18/220+ */
};


/*
* Stat Table (INT/WIS) -- Minimum failure rate (percentage)
*/
byte adj_mag_fail[] =
{
	99      /* 3 */,
	99      /* 4 */,
	99      /* 5 */,
	99      /* 6 */,
	99      /* 7 */,
	50      /* 8 */,
	30      /* 9 */,
	20      /* 10 */,
	15      /* 11 */,
	12      /* 12 */,
	11      /* 13 */,
	10      /* 14 */,
	9       /* 15 */,
	8       /* 16 */,
	7       /* 17 */,
	6       /* 18/00-18/09 */,
	6       /* 18/10-18/19 */,
	5       /* 18/20-18/29 */,
	5       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	4       /* 18/70-18/79 */,
	4       /* 18/80-18/89 */,
	3       /* 18/90-18/99 */,
	3       /* 18/100-18/109 */,
	2       /* 18/110-18/119 */,
	2       /* 18/120-18/129 */,
	2       /* 18/130-18/139 */,
	2       /* 18/140-18/149 */,
	1       /* 18/150-18/159 */,
	1       /* 18/160-18/169 */,
	1       /* 18/170-18/179 */,
	1       /* 18/180-18/189 */,
	1       /* 18/190-18/199 */,
	0       /* 18/200-18/209 */,
	0       /* 18/210-18/219 */,
	0       /* 18/220+ */
};


/*
* Stat Table (INT/WIS) -- Various things
*/
byte adj_mag_stat[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	17      /* 18/190-18/199 */,
	18      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
* Stat Table (CHA) -- payment percentages
*/
byte adj_cha_gold[] =
{
	130     /* 3 */,
	125     /* 4 */,
	122     /* 5 */,
	120     /* 6 */,
	118     /* 7 */,
	116     /* 8 */,
	114     /* 9 */,
	112     /* 10 */,
	110     /* 11 */,
	108     /* 12 */,
	106     /* 13 */,
	104     /* 14 */,
	103     /* 15 */,
	102     /* 16 */,
	101     /* 17 */,
	100     /* 18/00-18/09 */,
	99      /* 18/10-18/19 */,
	98      /* 18/20-18/29 */,
	97      /* 18/30-18/39 */,
	96      /* 18/40-18/49 */,
	95      /* 18/50-18/59 */,
	94      /* 18/60-18/69 */,
	93      /* 18/70-18/79 */,
	92      /* 18/80-18/89 */,
	91      /* 18/90-18/99 */,
	90      /* 18/100-18/109 */,
	89      /* 18/110-18/119 */,
	88      /* 18/120-18/129 */,
	87      /* 18/130-18/139 */,
	86      /* 18/140-18/149 */,
	85      /* 18/150-18/159 */,
	84      /* 18/160-18/169 */,
	83      /* 18/170-18/179 */,
	82      /* 18/180-18/189 */,
	81      /* 18/190-18/199 */,
	80      /* 18/200-18/209 */,
	79      /* 18/210-18/219 */,
	78      /* 18/220+ */
};


/*
* Stat Table (INT) -- Magic devices
*/
byte adj_int_dev[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	4       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	6       /* 18/60-18/69 */,
	6       /* 18/70-18/79 */,
	7       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	9       /* 18/110-18/119 */,
	10      /* 18/120-18/129 */,
	11      /* 18/130-18/139 */,
	12      /* 18/140-18/149 */,
	13      /* 18/150-18/159 */,
	14      /* 18/160-18/169 */,
	15      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	17      /* 18/190-18/199 */,
	18      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
* Stat Table (WIS) -- Saving throw
*/
byte adj_wis_sav[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	3       /* 18/30-18/39 */,
	3       /* 18/40-18/49 */,
	4       /* 18/50-18/59 */,
	4       /* 18/60-18/69 */,
	5       /* 18/70-18/79 */,
	5       /* 18/80-18/89 */,
	6       /* 18/90-18/99 */,
	7       /* 18/100-18/109 */,
	8       /* 18/110-18/119 */,
	9       /* 18/120-18/129 */,
	10      /* 18/130-18/139 */,
	11      /* 18/140-18/149 */,
	12      /* 18/150-18/159 */,
	13      /* 18/160-18/169 */,
	14      /* 18/170-18/179 */,
	15      /* 18/180-18/189 */,
	16      /* 18/190-18/199 */,
	17      /* 18/200-18/209 */,
	18      /* 18/210-18/219 */,
	19      /* 18/220+ */
};


/*
* Stat Table (DEX) -- disarming
*/
byte adj_dex_dis[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	0       /* 10 */,
	0       /* 11 */,
	0       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	4       /* 18/00-18/09 */,
	4       /* 18/10-18/19 */,
	4       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	5       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	5       /* 18/60-18/69 */,
	6       /* 18/70-18/79 */,
	6       /* 18/80-18/89 */,
	7       /* 18/90-18/99 */,
	8       /* 18/100-18/109 */,
	8       /* 18/110-18/119 */,
	8       /* 18/120-18/129 */,
	8       /* 18/130-18/139 */,
	8       /* 18/140-18/149 */,
	9       /* 18/150-18/159 */,
	9       /* 18/160-18/169 */,
	9       /* 18/170-18/179 */,
	9       /* 18/180-18/189 */,
	9       /* 18/190-18/199 */,
	10      /* 18/200-18/209 */,
	10      /* 18/210-18/219 */,
	10      /* 18/220+ */
};


/*
* Stat Table (INT) -- disarming
*/
byte adj_int_dis[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18/00-18/09 */,
	3       /* 18/10-18/19 */,
	3       /* 18/20-18/29 */,
	4       /* 18/30-18/39 */,
	4       /* 18/40-18/49 */,
	5       /* 18/50-18/59 */,
	6       /* 18/60-18/69 */,
	7       /* 18/70-18/79 */,
	8       /* 18/80-18/89 */,
	9       /* 18/90-18/99 */,
	10      /* 18/100-18/109 */,
	10      /* 18/110-18/119 */,
	11      /* 18/120-18/129 */,
	12      /* 18/130-18/139 */,
	13      /* 18/140-18/149 */,
	14      /* 18/150-18/159 */,
	15      /* 18/160-18/169 */,
	16      /* 18/170-18/179 */,
	17      /* 18/180-18/189 */,
	18      /* 18/190-18/199 */,
	19      /* 18/200-18/209 */,
	19      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
* Stat Table (DEX) -- bonus to ac (plus 128)
*/
byte adj_dex_ta[] =
{
	128 + -4        /* 3 */,
	128 + -3        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 1 /* 15 */,
	128 + 1 /* 16 */,
	128 + 1 /* 17 */,
	128 + 2 /* 18/00-18/09 */,
	128 + 2 /* 18/10-18/19 */,
	128 + 2 /* 18/20-18/29 */,
	128 + 2 /* 18/30-18/39 */,
	128 + 2 /* 18/40-18/49 */,
	128 + 3 /* 18/50-18/59 */,
	128 + 3 /* 18/60-18/69 */,
	128 + 3 /* 18/70-18/79 */,
	128 + 4 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
* Stat Table (STR) -- bonus to dam (plus 128)
*/
byte adj_str_td[] =
{
	128 + -2        /* 3 */,
	128 + -2        /* 4 */,
	128 + -1        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 2 /* 18/00-18/09 */,
	128 + 2 /* 18/10-18/19 */,
	128 + 3 /* 18/20-18/29 */,
	128 + 3 /* 18/30-18/39 */,
	128 + 3 /* 18/40-18/49 */,
	128 + 3 /* 18/50-18/59 */,
	128 + 3 /* 18/60-18/69 */,
	128 + 4 /* 18/70-18/79 */,
	128 + 5 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 10        /* 18/140-18/149 */,
	128 + 11        /* 18/150-18/159 */,
	128 + 12        /* 18/160-18/169 */,
	128 + 13        /* 18/170-18/179 */,
	128 + 14        /* 18/180-18/189 */,
	128 + 15        /* 18/190-18/199 */,
	128 + 16        /* 18/200-18/209 */,
	128 + 18        /* 18/210-18/219 */,
	128 + 20        /* 18/220+ */
};


/*
* Stat Table (DEX) -- bonus to hit (plus 128)
*/
byte adj_dex_th[] =
{
	128 + -3        /* 3 */,
	128 + -2        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + -1        /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 3 /* 18/00-18/09 */,
	128 + 3 /* 18/10-18/19 */,
	128 + 3 /* 18/20-18/29 */,
	128 + 3 /* 18/30-18/39 */,
	128 + 3 /* 18/40-18/49 */,
	128 + 4 /* 18/50-18/59 */,
	128 + 4 /* 18/60-18/69 */,
	128 + 4 /* 18/70-18/79 */,
	128 + 4 /* 18/80-18/89 */,
	128 + 5 /* 18/90-18/99 */,
	128 + 6 /* 18/100-18/109 */,
	128 + 7 /* 18/110-18/119 */,
	128 + 8 /* 18/120-18/129 */,
	128 + 9 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
* Stat Table (STR) -- bonus to hit (plus 128)
*/
byte adj_str_th[] =
{
	128 + -3        /* 3 */,
	128 + -2        /* 4 */,
	128 + -1        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 0 /* 15 */,
	128 + 0 /* 16 */,
	128 + 0 /* 17 */,
	128 + 1 /* 18/00-18/09 */,
	128 + 1 /* 18/10-18/19 */,
	128 + 1 /* 18/20-18/29 */,
	128 + 1 /* 18/30-18/39 */,
	128 + 1 /* 18/40-18/49 */,
	128 + 1 /* 18/50-18/59 */,
	128 + 1 /* 18/60-18/69 */,
	128 + 2 /* 18/70-18/79 */,
	128 + 3 /* 18/80-18/89 */,
	128 + 4 /* 18/90-18/99 */,
	128 + 5 /* 18/100-18/109 */,
	128 + 6 /* 18/110-18/119 */,
	128 + 7 /* 18/120-18/129 */,
	128 + 8 /* 18/130-18/139 */,
	128 + 9 /* 18/140-18/149 */,
	128 + 10        /* 18/150-18/159 */,
	128 + 11        /* 18/160-18/169 */,
	128 + 12        /* 18/170-18/179 */,
	128 + 13        /* 18/180-18/189 */,
	128 + 14        /* 18/190-18/199 */,
	128 + 15        /* 18/200-18/209 */,
	128 + 15        /* 18/210-18/219 */,
	128 + 16        /* 18/220+ */
};


/*
* Stat Table (STR) -- weight limit in deca-pounds
*/
byte adj_str_wgt[] =
{
	5       /* 3 */,
	6       /* 4 */,
	7       /* 5 */,
	8       /* 6 */,
	9       /* 7 */,
	10      /* 8 */,
	11      /* 9 */,
	12      /* 10 */,
	13      /* 11 */,
	14      /* 12 */,
	15      /* 13 */,
	16      /* 14 */,
	17      /* 15 */,
	18      /* 16 */,
	19      /* 17 */,
	20      /* 18/00-18/09 */,
	22      /* 18/10-18/19 */,
	24      /* 18/20-18/29 */,
	26      /* 18/30-18/39 */,
	28      /* 18/40-18/49 */,
	30      /* 18/50-18/59 */,
	31      /* 18/60-18/69 */,
	31      /* 18/70-18/79 */,
	32      /* 18/80-18/89 */,
	32      /* 18/90-18/99 */,
	33      /* 18/100-18/109 */,
	33      /* 18/110-18/119 */,
	34      /* 18/120-18/129 */,
	34      /* 18/130-18/139 */,
	35      /* 18/140-18/149 */,
	35      /* 18/150-18/159 */,
	36      /* 18/160-18/169 */,
	36      /* 18/170-18/179 */,
	37      /* 18/180-18/189 */,
	37      /* 18/190-18/199 */,
	38      /* 18/200-18/209 */,
	38      /* 18/210-18/219 */,
	39      /* 18/220+ */
};


/*
* Stat Table (STR) -- weapon weight limit in pounds
*/
byte adj_str_hold[] =
{
	4       /* 3 */,
	5       /* 4 */,
	6       /* 5 */,
	7       /* 6 */,
	8       /* 7 */,
	10      /* 8 */,
	12      /* 9 */,
	14      /* 10 */,
	16      /* 11 */,
	18      /* 12 */,
	20      /* 13 */,
	22      /* 14 */,
	24      /* 15 */,
	26      /* 16 */,
	28      /* 17 */,
	30      /* 18/00-18/09 */,
	30      /* 18/10-18/19 */,
	35      /* 18/20-18/29 */,
	40      /* 18/30-18/39 */,
	45      /* 18/40-18/49 */,
	50      /* 18/50-18/59 */,
	55      /* 18/60-18/69 */,
	60      /* 18/70-18/79 */,
	65      /* 18/80-18/89 */,
	70      /* 18/90-18/99 */,
	80      /* 18/100-18/109 */,
	80      /* 18/110-18/119 */,
	80      /* 18/120-18/129 */,
	80      /* 18/130-18/139 */,
	80      /* 18/140-18/149 */,
	90      /* 18/150-18/159 */,
	90      /* 18/160-18/169 */,
	90      /* 18/170-18/179 */,
	90      /* 18/180-18/189 */,
	90      /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
* Stat Table (STR) -- digging value
*/
byte adj_str_dig[] =
{
	0       /* 3 */,
	0       /* 4 */,
	1       /* 5 */,
	2       /* 6 */,
	3       /* 7 */,
	4       /* 8 */,
	4       /* 9 */,
	5       /* 10 */,
	5       /* 11 */,
	6       /* 12 */,
	6       /* 13 */,
	7       /* 14 */,
	7       /* 15 */,
	8       /* 16 */,
	8       /* 17 */,
	9       /* 18/00-18/09 */,
	10      /* 18/10-18/19 */,
	12      /* 18/20-18/29 */,
	15      /* 18/30-18/39 */,
	20      /* 18/40-18/49 */,
	25      /* 18/50-18/59 */,
	30      /* 18/60-18/69 */,
	35      /* 18/70-18/79 */,
	40      /* 18/80-18/89 */,
	45      /* 18/90-18/99 */,
	50      /* 18/100-18/109 */,
	55      /* 18/110-18/119 */,
	60      /* 18/120-18/129 */,
	65      /* 18/130-18/139 */,
	70      /* 18/140-18/149 */,
	75      /* 18/150-18/159 */,
	80      /* 18/160-18/169 */,
	85      /* 18/170-18/179 */,
	90      /* 18/180-18/189 */,
	95      /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
* Stat Table (STR) -- help index into the "blow" table
*/
byte adj_str_blow[] =
{
	3       /* 3 */,
	4       /* 4 */,
	5       /* 5 */,
	6       /* 6 */,
	7       /* 7 */,
	8       /* 8 */,
	9       /* 9 */,
	10      /* 10 */,
	11      /* 11 */,
	12      /* 12 */,
	13      /* 13 */,
	14      /* 14 */,
	15      /* 15 */,
	16      /* 16 */,
	17      /* 17 */,
	20 /* 18/00-18/09 */,
	30 /* 18/10-18/19 */,
	40 /* 18/20-18/29 */,
	50 /* 18/30-18/39 */,
	60 /* 18/40-18/49 */,
	70 /* 18/50-18/59 */,
	80 /* 18/60-18/69 */,
	90 /* 18/70-18/79 */,
	100 /* 18/80-18/89 */,
	110 /* 18/90-18/99 */,
	120 /* 18/100-18/109 */,
	130 /* 18/110-18/119 */,
	140 /* 18/120-18/129 */,
	150 /* 18/130-18/139 */,
	160 /* 18/140-18/149 */,
	170 /* 18/150-18/159 */,
	180 /* 18/160-18/169 */,
	190 /* 18/170-18/179 */,
	200 /* 18/180-18/189 */,
	210 /* 18/190-18/199 */,
	220 /* 18/200-18/209 */,
	230 /* 18/210-18/219 */,
	240 /* 18/220+ */
};


/*
* Stat Table (DEX) -- index into the "blow" table
*/
byte adj_dex_blow[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	1       /* 16 */,
	1       /* 17 */,
	1       /* 18/00-18/09 */,
	2       /* 18/10-18/19 */,
	2       /* 18/20-18/29 */,
	2       /* 18/30-18/39 */,
	2       /* 18/40-18/49 */,
	3       /* 18/50-18/59 */,
	3       /* 18/60-18/69 */,
	4       /* 18/70-18/79 */,
	4       /* 18/80-18/89 */,
	5       /* 18/90-18/99 */,
	6       /* 18/100-18/109 */,
	7       /* 18/110-18/119 */,
	8       /* 18/120-18/129 */,
	9       /* 18/130-18/139 */,
	10      /* 18/140-18/149 */,
	11      /* 18/150-18/159 */,
	12      /* 18/160-18/169 */,
	14      /* 18/170-18/179 */,
	16      /* 18/180-18/189 */,
	18      /* 18/190-18/199 */,
	20      /* 18/200-18/209 */,
	20      /* 18/210-18/219 */,
	20      /* 18/220+ */
};


/*
* Stat Table (DEX) -- chance of avoiding "theft" and "falling"
*/
byte adj_dex_safe[] =
{
	0       /* 3 */,
	1       /* 4 */,
	2       /* 5 */,
	3       /* 6 */,
	4       /* 7 */,
	5       /* 8 */,
	5       /* 9 */,
	6       /* 10 */,
	6       /* 11 */,
	7       /* 12 */,
	7       /* 13 */,
	8       /* 14 */,
	8       /* 15 */,
	9       /* 16 */,
	9       /* 17 */,
	10      /* 18/00-18/09 */,
	10      /* 18/10-18/19 */,
	15      /* 18/20-18/29 */,
	15      /* 18/30-18/39 */,
	20      /* 18/40-18/49 */,
	25      /* 18/50-18/59 */,
	30      /* 18/60-18/69 */,
	35      /* 18/70-18/79 */,
	40      /* 18/80-18/89 */,
	45      /* 18/90-18/99 */,
	50      /* 18/100-18/109 */,
	60      /* 18/110-18/119 */,
	70      /* 18/120-18/129 */,
	80      /* 18/130-18/139 */,
	90      /* 18/140-18/149 */,
	100     /* 18/150-18/159 */,
	100     /* 18/160-18/169 */,
	100     /* 18/170-18/179 */,
	100     /* 18/180-18/189 */,
	100     /* 18/190-18/199 */,
	100     /* 18/200-18/209 */,
	100     /* 18/210-18/219 */,
	100     /* 18/220+ */
};


/*
* Stat Table (CON) -- base regeneration rate
*/
byte adj_con_fix[] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	0       /* 10 */,
	0       /* 11 */,
	0       /* 12 */,
	0       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	1       /* 16 */,
	1       /* 17 */,
	2       /* 18/00-18/09 */,
	2       /* 18/10-18/19 */,
	2       /* 18/20-18/29 */,
	2       /* 18/30-18/39 */,
	2       /* 18/40-18/49 */,
	3       /* 18/50-18/59 */,
	3       /* 18/60-18/69 */,
	3       /* 18/70-18/79 */,
	3       /* 18/80-18/89 */,
	3       /* 18/90-18/99 */,
	4       /* 18/100-18/109 */,
	4       /* 18/110-18/119 */,
	5       /* 18/120-18/129 */,
	6       /* 18/130-18/139 */,
	6       /* 18/140-18/149 */,
	7       /* 18/150-18/159 */,
	7       /* 18/160-18/169 */,
	8       /* 18/170-18/179 */,
	8       /* 18/180-18/189 */,
	8       /* 18/190-18/199 */,
	9       /* 18/200-18/209 */,
	9       /* 18/210-18/219 */,
	9       /* 18/220+ */
};


/*
* Stat Table (CON) -- extra half-hitpoints per level (plus 128)
*/
byte adj_con_mhp[] =
{
	128 + -5        /* 3 */,
	128 + -3        /* 4 */,
	128 + -2        /* 5 */,
	128 + -1        /* 6 */,
	128 + 0 /* 7 */,
	128 + 0 /* 8 */,
	128 + 0 /* 9 */,
	128 + 0 /* 10 */,
	128 + 0 /* 11 */,
	128 + 0 /* 12 */,
	128 + 0 /* 13 */,
	128 + 0 /* 14 */,
	128 + 1 /* 15 */,
	128 + 1 /* 16 */,
	128 + 2 /* 17 */,
	128 + 3 /* 18/00-18/09 */,
	128 + 4 /* 18/10-18/19 */,
	128 + 4 /* 18/20-18/29 */,
	128 + 4 /* 18/30-18/39 */,
	128 + 4 /* 18/40-18/49 */,
	128 + 5 /* 18/50-18/59 */,
	128 + 6 /* 18/60-18/69 */,
	128 + 7 /* 18/70-18/79 */,
	128 + 8 /* 18/80-18/89 */,
	128 + 9 /* 18/90-18/99 */,
	128 + 10        /* 18/100-18/109 */,
	128 + 11        /* 18/110-18/119 */,
	128 + 12        /* 18/120-18/129 */,
	128 + 13        /* 18/130-18/139 */,
	128 + 14        /* 18/140-18/149 */,
	128 + 15        /* 18/150-18/159 */,
	128 + 16        /* 18/160-18/169 */,
	128 + 18        /* 18/170-18/179 */,
	128 + 20        /* 18/180-18/189 */,
	128 + 22        /* 18/190-18/199 */,
	128 + 25        /* 18/200-18/209 */,
	128 + 26        /* 18/210-18/219 */,
	128 + 27        /* 18/220+ */
};


/*
* This table is used to help calculate the number of blows the player can
* make in a single round of attacks (one player turn) with a normal weapon.
*
* This number ranges from a single blow/round for weak players to up to six
* blows/round for powerful warriors.
*
* Note that certain artefacts and ego-items give "bonus" blows/round.
*
* First, from the player class, we extract some values:
*
*    Warrior --> num = 6; mul = 5; div = MAX(30, weapon_weight);
*    Mage    --> num = 4; mul = 2; div = MAX(40, weapon_weight);
*    Priest  --> num = 5; mul = 3; div = MAX(35, weapon_weight);
*    Rogue   --> num = 5; mul = 3; div = MAX(30, weapon_weight);
*    Ranger  --> num = 5; mul = 4; div = MAX(35, weapon_weight);
*    Paladin --> num = 5; mul = 4; div = MAX(30, weapon_weight);
*
* To get "P", we look up the relevant "adj_str_blow[]" (see above),
* multiply it by "mul", and then divide it by "div", rounding down.
*
* To get "D", we look up the relevant "adj_dex_blow[]" (see above),
* note especially column 6 (DEX 18/101) and 11 (DEX 18/150).
*
* The player gets "blows_table[P][D]" blows/round, as shown below,
* up to a maximum of "num" blows/round, plus any "bonus" blows/round.
*/
byte blows_table[12][12] =
{
/* P/D */
/* 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11+ */

/* 0  */
{  1,   1,   1,   1,   1,   1,   2,   2,   2,   2,   2,   3 },

/* 1  */
{  1,   1,   1,   1,   2,   2,   3,   3,   3,   4,   4,   4 },

/* 2  */
{  1,   1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5 },

/* 3  */
{  1,   2,   2,   3,   3,   4,   4,   4,   5,   5,   5,   5 },

/* 4  */
{  1,   2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5 },

/* 5  */
{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

/* 6  */
{  2,   2,   3,   3,   4,   4,   5,   5,   5,   5,   5,   6 },

/* 7  */
{  2,   3,   3,   4,   4,   4,   5,   5,   5,   5,   5,   6 },

/* 8  */
{  3,   3,   3,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

/* 9  */
{  3,   3,   4,   4,   4,   4,   5,   5,   5,   5,   6,   6 },

/* 10 */
{  3,   3,   4,   4,   4,   4,   5,   5,   5,   6,   6,   6 },

/* 11+ */
{  3,   3,   4,   4,   4,   4,   5,   5,   6,   6,   6,   6 },
};



/*
* Store owners (exactly four "possible" owners per store, chosen randomly)
* { name, purse, max greed, min greed, haggle_per, tolerance, race }
*/
owner_type owners[MAX_STORES][MAX_OWNERS] =
{
	{
		/* Store 0 (General Store)*/
		{ "Wirt the Weasel",         250,    170, 108,  5, 15, RACE_HUMAN}, /* Diablo II + Diablo I */
		{ "Atma the Tragic",       500,    175, 108,  4, 12, RACE_HUMAN}, /* Diablo II ( should be an Inn Owner, but I will not put in a hack on a hack for her */
		{ "Elzix the Scoundrel",          750,    170, 107,  5, 15, RACE_HUMAN}, /* Diablo II  */
		{ "Ogden the Innkeeper",        1000,    165, 107,  6, 18, RACE_HUMAN},  /* Diablo I  */
		{ "Wirt the Weasel",         250,    170, 108,  5, 15, RACE_HUMAN}, /* Diablo II + Diablo I */
		{ "Atma the Tragic",       500,    175, 108,  4, 12, RACE_HUMAN}, /* Diablo II ( should be an Inn Owner, but I will not put in a hack on a hack for her */
		{ "Elzix the Scoundrel",          750,    170, 107,  5, 15, RACE_HUMAN}, /* Diablo II  */
		{ "Ogden the Innkeeper",        1000,    165, 107,  6, 18, RACE_HUMAN},  /* Diablo I  */
		{ "Wirt the Weasel",         250,    170, 108,  5, 15, RACE_HUMAN}, /* Diablo II + Diablo I */
		{ "Atma the Tragic",       500,    175, 108,  4, 12, RACE_HUMAN}, /* Diablo II ( should be an Inn Owner, but I will not put in a hack on a hack for her */
		{ "Elzix the Scoundrel",          750,    170, 107,  5, 15, RACE_HUMAN}, /* Diablo II  */
		{ "Ogden the Innkeeper",        1000,    165, 107,  6, 18, RACE_HUMAN},  /* Diablo I  */
		{ "Wirt the Weasel",         250,    170, 108,  5, 15, RACE_HUMAN}, /* Diablo II + Diablo I */
		{ "Atma the Tragic",       500,    175, 108,  4, 12, RACE_HUMAN}, /* Diablo II ( should be an Inn Owner, but I will not put in a hack on a hack for her */
		{ "Elzix the Scoundrel",          750,    170, 107,  5, 15, RACE_HUMAN}, /* Diablo II  */
		{ "Ogden the Innkeeper",        1000,    165, 107,  6, 18, RACE_HUMAN},  /* Diablo I  */
	},
	{
		/* Store 1 (Armoury) */
		{ "Flavie, Rogue Guardian",           10000,   210, 115,  5,  7, RACE_HUMAN},  /* Diablo II */
		{ "Flava, Holy Knight",          15000,  190, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Hratli, Sorcer",            25000,  200, 112,  4, 10, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Feradach Dathi",          30000,  200, 112,  4,  5, RACE_HUMAN},  /* Irish King, enchanted his armor with a speed bonus ;) What a guy */
		{ "Flavie, Rogue Guardian",           10000,   210, 115,  5,  7, RACE_HUMAN},  /* Diablo II */
		{ "Flava, Holy Knight",          15000,  190, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Hratli, Sorcer",            25000,  200, 112,  4, 10, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Feradach Dathi",          30000,  200, 112,  4,  5, RACE_HUMAN},  /* Irish King, enchanted his armor with a speed bonus ;) What a guy */
		{ "Flavie, Rogue Guardian",           10000,   210, 115,  5,  7, RACE_HUMAN},  /* Diablo II */
		{ "Flava, Holy Knight",          15000,  190, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Hratli, Sorcer",            25000,  200, 112,  4, 10, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Feradach Dathi",          30000,  200, 112,  4,  5, RACE_HUMAN},  /* Irish King, enchanted his armor with a speed bonus ;) What a guy */
		{ "Flavie, Rogue Guardian",           10000,   210, 115,  5,  7, RACE_HUMAN},  /* Diablo II */
		{ "Flava, Holy Knight",          15000,  190, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Hratli, Sorcer",            25000,  200, 112,  4, 10, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Feradach Dathi",          30000,  200, 112,  4,  5, RACE_HUMAN},  /* Irish King, enchanted his armor with a speed bonus ;) What a guy */
	},
	{
		/* Store 2 (Weaponsmith)*/
		{ "Charsi, Rogue Blacksmith",      10000,   210, 115,  6,  6, RACE_HUMAN}, /* Diablo II ,  */
		{ "Kashya, Rogue Captain",        15000,  185, 110,  5,  9, RACE_HUMAN},  /* Diablo II */
		{ "Geglash the Barbarian",         25000,  190, 115,  5,  7, RACE_BARBARIAN}, /* Diablo II, act 2*/
		{ "Lady of the Lake",       30000,  195, 112,  4,  8, RACE_SPRITE}, /*  She was in the business of selling *magical* blades */
		{ "Natalya, Hunter of Evil",      10000,   210, 115,  6,  6, RACE_HUMAN},  /* Diablo II, act 3*/
		{ "Larzuk the Barbarian",        15000,  185, 110,  5,  9, RACE_BARBARIAN}, /* Diablo II, act 5*/
		{ "Qual-Kehk the Troubled",         25000,  190, 115,  5,  7, RACE_HUMAN}, /* Diablo II, act 5*/
		{ "Griswold, Blacksmith",       30000,  195, 112,  4,  8, RACE_DWARF}, /* Diablo I */
		{ "Charsi, Rogue Blacksmith",      10000,   210, 115,  6,  6, RACE_HUMAN}, /* Diablo II ,  */
		{ "Kashya, Rogue Captain",        15000,  185, 110,  5,  9, RACE_HUMAN},  /* Diablo II */
		{ "Geglash the Barbarian",         25000,  190, 115,  5,  7, RACE_BARBARIAN}, /* Diablo II, act 2*/
		{ "Lady of the Lake",       30000,  195, 112,  4,  8, RACE_SPRITE}, /*  She was in the business of selling *magical* blades */
		{ "Natalya, Hunter of Evil",      10000,   210, 115,  6,  6, RACE_HUMAN},  /* Diablo II, act 3*/
		{ "Larzuk the Barbarian",        15000,  185, 110,  5,  9, RACE_BARBARIAN}, /* Diablo II, act 5*/
		{ "Qual-Kehk the Troubled",         25000,  190, 115,  5,  7, RACE_HUMAN}, /* Diablo II, act 5*/
		{ "Griswold, Blacksmith",       30000,  195, 112,  4,  8, RACE_DWARF}, /* Diablo I */
	},
		{
		/* Store 3 (Temple)*/
		{ "Akara, Rogue Priestess",         10000,   175, 109,  6, 15, RACE_HUMAN}, /*Diablo II , The spiritual leader of the Sisters of the Sightless Eye*/
		{ "Lord Jerhyn",         15000,  185, 110,  5, 23, RACE_HUMAN}, /* Diablo II, act 2 */
		{ "Ormus, Healer",           25000,  180, 107,  6, 20, RACE_HUMAN},  /* Diablo II, act 3 */
		{ "Asenath the Holy",          30000,  185, 109,  5, 15, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Pepin the Healer",         10000,   175, 109,  6, 15, RACE_HUMAN}, /* Diablo I  */
		{ "Julian the Theurgist",         15000,  185, 110,  5, 23, RACE_HUMAN},  /* Theurgists are cool ? */
		{ "Roger Bacon",           25000,  180, 107,  6, 20, RACE_HUMAN},  /* Yah, those Franciscans eh ! */
		{ "Abraham ben Samuel Abulafia",          30000,  185, 109,  5, 15, RACE_HUMAN},  /* Go kabbalah's */
		{ "Akara, Rogue Priestess",         10000,   175, 109,  6, 15, RACE_HUMAN}, /*Diablo II , The spiritual leader of the Sisters of the Sightless Eye*/
		{ "Lord Jerhyn",         15000,  185, 110,  5, 23, RACE_HUMAN}, /* Diablo II, act 2 */
		{ "Ormus, Healer",           25000,  180, 107,  6, 20, RACE_HUMAN},  /* Diablo II, act 3 */
		{ "Asenath the Holy",          30000,  185, 109,  5, 15, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Pepin the Healer",         10000,   175, 109,  6, 15, RACE_HUMAN}, /* Diablo I  */
		{ "Julian the Theurgist",         15000,  185, 110,  5, 23, RACE_HUMAN},  /* Theurgists are cool ? */
		{ "Roger Bacon",           25000,  180, 107,  6, 20, RACE_HUMAN},  /* Yah, those Franciscans eh ! */
		{ "Abraham ben Samuel Abulafia",          30000,  185, 109,  5, 15, RACE_HUMAN},  /* Go kabbalah's */
	},
	{
		/* Store 4 (Alchemist)*/
		{ "Lysander",				10000,  190, 111,  5,  8, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Alkor",         10000,  190, 110,  6,  8, RACE_HUMAN},  /* Diablo II, act 3 */
		{ "Jamella, Gatekeeper",           15000,  200, 116,  6,  9, RACE_HUMAN}, /* Diablo II, act 4 */
		{ "Anya, the Fated",       15000,  220, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 5 */
		{ "Malah, town Nurse",         10000,  190, 111,  5,  8, RACE_DWARF}, /* Diablo II, act 5 */
		{ "Twardowski, Physician",         10000,  190, 110,  6,  8, RACE_HUMAN},  /*  Famous Alchemist  */
		{ "Parmigianino",           15000,  200, 116,  6,  9, RACE_HUMAN},  /*  Girolamo Francesco Maria Mazzola (11 January 1503- 24 August 1540) */
		{ "Nicholas Flamel",       15000,  220, 111,  4,  9, RACE_SPRITE},  /* Okay, this is _not_ the NF of Harry Potter ;) */
		{ "Lysander",				10000,  190, 111,  5,  8, RACE_HUMAN},  /* Diablo II, act 2 */
		{ "Alkor",         10000,  190, 110,  6,  8, RACE_HUMAN},  /* Diablo II, act 3 */
		{ "Jamella, Gatekeeper",           15000,  200, 116,  6,  9, RACE_HUMAN}, /* Diablo II, act 4 */
		{ "Anya, the Fated",       15000,  220, 111,  4,  9, RACE_HUMAN},  /* Diablo II, act 5 */
		{ "Malah, town Nurse",         10000,  190, 111,  5,  8, RACE_DWARF}, /* Diablo II, act 5 */
		{ "Twardowski, Physician",         10000,  190, 110,  6,  8, RACE_HUMAN},  /*  Famous Alchemist  */
		{ "Parmigianino",           15000,  200, 116,  6,  9, RACE_HUMAN},  /*  Girolamo Francesco Maria Mazzola (11 January 1503- 24 August 1540) */
		{ "Nicholas Flamel",       15000,  220, 111,  4,  9, RACE_SPRITE},  /* Okay, this is _not_ the NF of Harry Potter ;) */
	},
	{
		/* Store 5 (Magic)*/
		{ "Merlin the Sage",       30000,  175, 110,  5, 11, RACE_DEVILSPAWN},		/*Merlin was the son of an incubus*/
    	{ "Lady of the Lake",        15000,  200, 110,  7,  8, RACE_SPRITE},	     /*  She was in the business of selling *magical* blades */
 		{ "Drognan",        15000,  200, 110,  7,  8, RACE_HUMAN},  /* Diablo II */
		{ "Adria the Witch",         20000,  215, 113,  6, 10, RACE_HUMAN},  /* Diablo I */
		{ "Witch of Endor",       30000,  200, 110,  7, 10, RACE_HUMAN}, /* Hebrew Bible */
		{ "Gyges of Lydia",       30000,  175, 110,  5, 11, RACE_HUMAN}, /* king said to possess magical artifacts */
		{ "Iannes and Mambres",         20000,  215, 113,  6, 10, RACE_MIND_FLAYER}, /* magicians at Pharaoh's court mentioned in the New Testament */
		{ "Simon the Sorcerer",       30000,  200, 110,  7, 10, RACE_DEVILSPAWN}, /* Simon Magus, not the computer game */
		{ "Merlin the Sage",       30000,  175, 110,  5, 11, RACE_DEVILSPAWN},		/*Merlin was the son of an incubus*/
    	{ "Lady of the Lake",        15000,  200, 110,  7,  8, RACE_SPRITE},	     /*  She was in the business of selling *magical* blades */
 		{ "Drognan",        15000,  200, 110,  7,  8, RACE_HUMAN},  /* Diablo II */
		{ "Adria the Witch",         20000,  215, 113,  6, 10, RACE_HUMAN},  /* Diablo I */
		{ "Witch of Endor",       30000,  200, 110,  7, 10, RACE_HUMAN}, /* Hebrew Bible */
		{ "Gyges of Lydia",       30000,  175, 110,  5, 11, RACE_HUMAN}, /* king said to possess magical artifacts */
		{ "Iannes and Mambres",         20000,  215, 113,  6, 10, RACE_MIND_FLAYER}, /* magicians at Pharaoh's court mentioned in the New Testament */
		{ "Simon the Sorcerer",       30000,  200, 110,  7, 10, RACE_DEVILSPAWN}, /* Simon Magus, not the computer game */
	},
	{
		/* Store 6 (Black Market)*/
		{ "Apollonius of Tyana", 20000,  250, 150, 10,  5, RACE_HUMAN},  /* Year 2 */
		{ "Lady of the Lake",          20000,  250, 150, 10,  5, RACE_SPRITE},  /*  She was pretty shady, in the business of selling *magical* *blades* */
		{ "Gheed - Caravan Trader",          30000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II */
		{ "Warriv - Caravan Leader",           30000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II */
		{ "Greiz - Mercenary Captain",             20000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II, act 2 */
		{ "Asheara, Iron Wolf",          20000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Lucius Apuleius",          30000,  250, 150, 10,  5, RACE_HUMAN}, /* A Discourse on Magic ;) */
		{ "Elymas",           30000,  250, 150, 10,  5, RACE_HUMAN}, /* Bibilic Jewish Magician */
		{ "Apollonius of Tyana", 20000,  250, 150, 10,  5, RACE_HUMAN},  /* Year 2 */
		{ "Lady of the Lake",          20000,  250, 150, 10,  5, RACE_SPRITE},  /*  She was pretty shady, in the business of selling *magical* *blades* */
		{ "Gheed - Caravan Trader",          30000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II */
		{ "Warriv - Caravan Leader",           30000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II */
		{ "Greiz - Mercenary Captain",             20000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II, act 2 */
		{ "Asheara, Iron Wolf",          20000,  250, 150, 10,  5, RACE_HUMAN}, /* Diablo II, act 3 */
		{ "Lucius Apuleius",          30000,  250, 150, 10,  5, RACE_HUMAN}, /* A Discourse on Magic ;) */
		{ "Elymas",           30000,  250, 150, 10,  5, RACE_HUMAN}, /* Bibilic Jewish Magician */	
    },
	{
		/* Store 7 (Home)*/
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
		{ "Your home",                          0,      100, 100,  0, 99, 99},
	},
	{
		/*Eliezer of Worms*/
		/* Store 8 (Bookstore)*/
		{ "Johann Weyer", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* Pseudomonarchia Daemonum (between February 24, 1515 and February 24, 1516 � February 24, 1588)*/
		{ "Ludovico Antonio Muratori", 20000, 120, 105, 6, 16, RACE_HALF_ELF},  /* Ludovico Antonio Muratori (1672 - 1750) was an Italian historian */
		{ "Johannes Trithemius", 25000, 120, 110, 7, 19, RACE_HUMAN}, /* Steganographia  (1 February 1462 - 13 December 1516) */
		{ "Abra-Melin", 30000, 140, 105, 6, 12, RACE_HUMAN}, /* The Book of the Sacred Magic of Abramelin the Mage  - 1458 */
		{ "Alibek the Egyptian", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* The Grand Grimoir - 13th century */
		{ "John Dee", 20000, 120, 105, 6, 16, RACE_HUMAN}, /* Wrote books in the enochian language - (July 13, 1527 � 1608 or 1609)*/
		{ "Heinrich Cornelius Agrippa", 25000, 120, 110, 7, 19, RACE_HUMAN}, /* Libri tres de occulta philosophia - born of noble birth in Cologne September 14, 1486, died in Grenoble February 18, 1535) */
		{ "Peter de Abano", 30000, 140, 105, 6, 12, RACE_HUMAN}, /* Heptameron - (c. 1250-1316)  */
		{ "Hypatia of Alexandria", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* philosopher, mathematician, and teacher ,  (c. 370 - 415) */
		{ "Eratosthenes", 20000, 120, 105, 6, 16, RACE_HUMAN}, /* Famous Librarian - (276 BC - 194 BC)*/
		{ "Gaius Suetonius Tranquillus", 25000, 120, 110, 7, 19, RACE_HUMAN}, /* Roman Librarian -  (69 or 70 AD - after 130 AD  */
		{ "Zenodotus", 30000, 140, 105, 6, 12, RACE_HUMAN},  /* He was the first superintendent of the library at Alexandria - 280 BC. */
		{ "Flavius Magnus Aurelius Cassiodorus Senator", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* Owner of east,west,greek,roman, goth works */
		{ "Dionysius Exiguus", 20000, 120, 105, 6, 16, RACE_HUMAN}, /* Inventor of Anno Domini */
		{ "Callimachus", 25000, 120, 110, 7, 19, RACE_YEEK}, /*  His Pinakes (tablets), in 120 books, a critical and chronologically arranged catalogue of the library, laid the foundation of a history of Greek literature. (ca. 305 BC- ca. 240 BC)  */
		{ "Demetrius Phalereus", 30000, 140, 105, 6, 12, RACE_GNOME}, /* 40,000 to 700,000 books and was initially organized by Demetrius Phalereus. - 3rd century BC, */
	},
	{
		/* Store 9 (Inn) , This shop has a hack that prevents (Human to show) , shop is identified by nymber*/
		{ "The George and Dragon", 15000, 175, 108, 4, 12, RACE_HUMAN}, /*London*/
		{ "The Tabard Inn", 20000, 120, 105, 6, 16, RACE_HUMAN},  /* London */
 		{ "The Cat and The Fiddle", 25000, 120, 110, 7, 19, RACE_HUMAN},  /* Hampshire */
 		{ "The Old Bell", 30000, 140, 105, 6, 12, RACE_HUMAN},  /* Malmesbury */
		{ "The Weary Friar", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* Cornwall */
		{ "The Fighting Cocks", 20000, 120, 105, 6, 16, RACE_HUMAN}, /* St Albans */
		{ "The Trip to Jerusalem", 25000, 120, 110, 7, 19, RACE_HUMAN}, /* Nottingham */
		{ "The Bull & Bladder", 30000, 140, 105, 6, 12, RACE_HUMAN}, /* Birmingham */
		{ "The George and Dragon", 15000, 175, 108, 4, 12, RACE_HUMAN}, /*London*/
		{ "The Tabard Inn", 20000, 120, 105, 6, 16, RACE_HUMAN},  /* London */
 		{ "The Cat and The Fiddle", 25000, 120, 110, 7, 19, RACE_HUMAN},  /* Hampshire */
 		{ "The Old Bell", 30000, 140, 105, 6, 12, RACE_HUMAN},  /* Malmesbury */
		{ "The Weary Friar", 15000, 175, 108, 4, 12, RACE_HUMAN}, /* Cornwall */
		{ "The Fighting Cocks", 20000, 120, 105, 6, 16, RACE_HUMAN}, /* St Albans */
		{ "The Trip to Jerusalem", 25000, 120, 110, 7, 19, RACE_HUMAN}, /* Nottingham */
		{ "The Bull & Bladder", 30000, 140, 105, 6, 12, RACE_HUMAN}, /* Birmingham */
	},
	{
		/* Store 10 (Hall)*/
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
		{ "Hall of Records",                          0,      100, 100,  0, 99, 99},
	},
	{
		/* Store 11 (Pawnbroker)* Yah, I just doubled the entries, enterprising souls can find some more known trader states */
		{ "Joris the Fleming", 2000, 100, 100, 4, 12, RACE_HUMAN},  /*Famous traders ;) */
		{ "Giovanni the Venetian", 4000, 100, 100, 6, 16, RACE_HUMAN}, /* Mercantile Italian city-state */
		{ "Hiram of Tyre", 5000, 100, 100, 7, 19, RACE_HUMAN},  /*Phoenicia was an ancient civilization in the north of ancient Canaan, trading nation, , furnished architects, workmen and cedar timbers for the temple of his ally Solomon at Jerusalem.*/
		{ "Yosef the Radhanite", 10000, 100, 100, 6, 12, RACE_HUMAN}, /* Jewish Trading clan */
		{ "Nicolasthe Genoan", 2000, 100, 100, 4, 12, RACE_HUMAN},  /*mercantile Italian city-stat*/
		{ "Alfonso the Pisan", 4000, 100, 100, 6, 16, RACE_HUMAN}, /*mercantile Italian city-state*/
		{ "Eduardo the Amalfian", 5000, 100, 100, 7, 19, RACE_HUMAN},  /*mercantile Italian city-state*/
		{ "Jan the Dutch", 10000, 100, 100, 6, 12, RACE_HUMAN}, /* Vikings also traded when they couldnt steal */
		{ "Joris the Fleming", 2000, 100, 100, 4, 12, RACE_HUMAN},  /*Famous traders ;) */
		{ "Giovanni the Venetian", 4000, 100, 100, 6, 16, RACE_HUMAN}, /* Mercantile Italian city-state */
		{ "Hiram of Tyre", 5000, 100, 100, 7, 19, RACE_HUMAN},  /*Phoenicia was an ancient civilization in the north of ancient Canaan, trading nation, , furnished architects, workmen and cedar timbers for the temple of his ally Solomon at Jerusalem.*/
		{ "Yosef the Radhanite", 10000, 100, 100, 6, 12, RACE_HUMAN}, /* Jewish Trading clan */
		{ "Nicolasthe Genoan", 2000, 100, 100, 4, 12, RACE_HUMAN},  /*mercantile Italian city-stat*/
		{ "Alfonso the Pisan", 4000, 100, 100, 6, 16, RACE_HUMAN}, /*mercantile Italian city-state*/
		{ "Eduardo the Amalfian", 5000, 100, 100, 7, 19, RACE_HUMAN},  /*mercantile Italian city-state*/
		{ "Cain the Elder", 10000, 100, 100, 6, 12, RACE_HUMAN}, /* Diablo II */
	},
};




/*
* This table allows quick conversion from "speed" to "energy"
* The basic function WAS ((S>=110) ? (S-110) : (100 / (120-S)))
* Note that table access is *much* quicker than computation.
*
* Note that the table has been changed at high speeds.  From
* "Slow (-40)" to "Fast (+30)" is pretty much unchanged, but
* at speeds above "Fast (+30)", one approaches an asymptotic
* effective limit of 50 energy per turn.  This means that it
* is relatively easy to reach "Fast (+30)" and get about 40
* energy per turn, but then speed becomes very "expensive",
* and you must get all the way to "Fast (+50)" to reach the
* point of getting 45 energy per turn.  After that point,
* furthur increases in speed are more or less pointless,
* except to balance out heavy inventory.
*
* Note that currently the fastest monster is "Fast (+30)".
*
* It should be possible to lower the energy threshhold from
* 100 units to 50 units, though this may interact badly with
* the (compiled out) small random energy boost code.  It may
* also tend to cause more "clumping" at high speeds.
*/

/* This table has been inverted for Hellband. The new values
* are 1000/x where x is the old value. This gives the same spread
* but subtracting extract_energy[n] each move and always adding
* 10 per turn, rather than adding extract_energy[n] each turn and
* always subtracting 100.
*
* This has been done to allow the seperating out of movement speed
* and attack speed.
*/
u16b extract_energy[200] =
{
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* Slow */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* S-50 */     1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,  1000,
/* S-40 */     500,  500,  500,  500,  500,  500,  500,  500,  500,  500,
/* S-30 */     500,  500,  500,  500,  500,  500,  500,  333,  333,  333,
/* S-20 */     333,  333,  333,  333,  333,  250,  250,  250,  250,  250,
/* S-10 */     200,  200,  200,  200,  167,  167,  143,  143,  125,  111,
/* Norm */    100, 91, 83, 77, 71, 67, 63, 59, 56, 53,
/* F+10 */    50, 48, 45, 43, 42, 40, 38, 37, 36, 34,
/* F+20 */    33, 32, 31, 30, 29, 29, 28, 28, 27, 27,
/* F+30 */    26, 26, 26, 26, 25, 25, 25, 24, 24, 24,
/* F+40 */    24, 24, 24, 23, 23, 23, 23, 23, 23, 23,
/* F+50 */    22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
/* F+60 */    21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
/* F+70 */    20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
/* Fast */    20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
};







/*
* Base experience levels, may be adjusted up for race and/or class
*/
s32b player_exp[PY_MAX_LEVEL] =
{
	10,
	25,
	45,
	70,
	100,
	140,
	200,
	280,
	380,
	500,
	650,
	850,
	1100,
	1400,
	1800,
	2300,
	2900,
	3600,
	4400,
	5400,
	6800,
	8400,
	10200,
	12500,
	17500,
	25000,
	35000L,
	50000L,
	75000L,
	100000L,
	150000L,
	200000L,
	275000L,
	350000L,
	450000L,
	550000L,
	700000L,
	850000L,
	1000000L,
	1250000L,
	1500000L,
	1800000L,
	2100000L,
	2400000L,
	2700000L,
	3000000L,
	3500000L,
	4000000L,
	4500000L,
	5000000L
};


/*
* Player Sexes
*
*      Title,
*      Winner
*/
player_sex sex_info[MAX_SEXES] =
{
	{
		"Female",
		"Queen"
	},
	{
		"Male",
		"King"
	}
};


/*
* Player Races
*
*      Title,
*      {STR,INT,WIS,DEX,CON,CHA},
*      r_dis, r_dev, r_sav, r_stl, r_srh, r_fos, r_thn, r_thb,
*      hitdie, exp base,
*      Age (Base, Mod),
*      Male (Hgt, Wgt),
*      Female (Hgt, Wgt)
*      infra,
*      class-choices
*/
player_race race_info[MAX_RACES] =
{
	{
		"Barbarian",
		{ 3, -2,  -1,  1,  2, -2 },
		-2, -10, 2,  -1,  1, 7, 12, 10,
		11, 120,
		14, 8,
		82, 5, 200, 20,
		78,  6, 190, 15,
		0,
		0x089D,
	},
	{
		"Cyclops",
		{ 4, -3, -3, -3, 4, -6 },
		-4, -5, -5, -2, -2, 5, 20, 12,
		13, 130,
		50, 24,
		92, 10, 255, 60,
		80,  8, 235, 60,
		1,
		0x0005,
	},
	{
		"Dark-Elf",
		{ -1, 3, 2, 2, -2, 1 },
		5, 15, 20, 3, 8, 12, -5, 10,
		9, 150,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		5,
		0x1FDF,
	},
	{
		"Devilspawn",
		{ 2, -2, -1, -1, 2, -4 },
		-5, -2, -1, -1, -1, 5, 12, 5,
		11, 140,
		14, 6,
		65,  6, 150, 20,
		61,  6, 120, 15,
		0,
		0x1FCF,
	},
	{
		"Draconian",
		{ 2, 1, 1, 1, 2, -3 },
		-2, 5, 3, 0, 1, 10, 5, 5,
		11, 250,
		75, 33,
		76,  1, 160,  5,
		72,  1, 130,  5,
		2,
		0x1F57,
	},
	{
		"Dwarf",
		{  2, -2,  2, -2,  2, -3 },
		2,  9,  10,  -1,  7,  10, 15,  0,
		11,  125,
		35, 15,
		48,  3, 150, 10,
		46,  3, 120, 10,
		5,
		0x0805,        
	},
	{
		"Elf",
		{ -1,  2,  2,  1, -2,  2 },
		5,  6,  6,  2, 8,  12, -5, 15,
		8,  120,
		75, 75,
		60,  4, 100,  6,
		54,  4, 80,  6,
		3,
		0x1F5F,
	},
	{
		"Gnome",
		{ -1,  2,  0,  2,  1, -2 },
		10, 12, 12,  3, 6,  13, -8, 12,
		8,  135,
		50, 40,
		42,  3, 90,  6,
		39,  3, 75,  3,
		4,
		0x1E0F,
	},
	{
		"Golem",
		{ 4, -5, -5, 0, 4, -4 },
		-5, -5, 10, -1, -1, 8, 20, 0,
		12, 200,
		1, 100,
		66,  1, 200,  6,
		62,  1, 180,  6,
		4,
		0x0001,
	},
	{
		"Half-Elf",
		{ -1,  1,  1,  1, -1,  1 },
		2,  3,  3,  1, 6,  11, -1,  5,
		9,  110,
		24, 16,
		66,  6, 130, 15,
		62,  6, 100, 10,
		2,
		0x1FFF,
	},
	{
		"Half-Giant",
		{ 4, -2, -2, -2, 3, -3 },
		-6, -8, -6, -2, -1, 5, 25, 5,
		13, 150,
		40, 10,
		100,10, 255, 65,
		80, 10, 240, 64,
		3,
		0x0011,
	},
	{
		"Half-Ogre",
		{ 3, -1, -1, -1, 3, -3 },
		-3, -5, -5, -2, -1, 5, 20, 0,
		12,  130,
		40, 10,
		92, 10, 255, 60,
		80,  8, 235, 60,
		3,
		0x0C07,
	},
	{
		"Half-Orc",
		{  2, -1,  0,  0,  1, -4 },
		-3, -3, -3,  -1,  0, 7, 12, -5,
		10,  110,
		11,  4,
		66,  1, 150,  5,
		62,  1, 120,  5,
		3,
		0x098D,
	},
	{
		"Half-Titan",
		{ 5, 1, 1, -2, 3, 1 },
		-5, 5, 2, -2, 1, 8, 25, 0,
		14, 255,
		100,30,
		111, 11, 255, 86,
		99, 11, 250, 86,
		0,
		0x1F27,
	},
	{
		"Half-Troll",
		{ 4, -4, -2, -4,  3, -6 },
		-5, -8, -8, -2,  -1, 5, 20, -10,
		12,  137,
		20, 10,
		96, 10, 250, 50,
		84,  8, 225, 40,
		3,
		0x0805,
	},
	{
		"High-Elf",
		{  1,  3,  2,  3,  1,  5 },
		4,  20, 20,  4,  3, 14, 10, 25,
		10,  200,
		100, 30,
		90, 10, 190, 20,
		82, 10, 180, 15,
		4,
		0x1F5F,
	},
	{
		"Hobbit",
		{ -2,  2,  1,  3,  2,  1 },
		15, 18, 18, 5, 12,  15, -10, 20,
		7,  110,
		21, 12,
		36,  3, 60,  3,
		33,  3, 50,  3,
		4,
		0x1C0B,
	},
	{
		"Human",
		{  0,  0,  0,  0,  0,  0 },
		0,  0,  0,  0,  0,  10,  0,  0,
		10,  100,
		14,  6,
		72,  6, 180, 25,
		66,  4, 150, 20,
		0,
		0x1FFF,
	},
	{
		"Imp",
		{ -1, -1, -1, 1, 2, -3 },
		-3, 2, -1, 1, -1, 10, 5, -5,
		10, 110,
		13,  4,
		68,  1, 150,  5,
		64,  1, 120,  5,
		3,
		0x17CB,
	},
	{
		"Klackon",
		{ 2, -1, -1, 1, 2, -2 },
		10, 5, 5, 0, -1, 10, 5, 5,
		12, 135,
		20, 3,
		60,  3, 80,  4,
		54,  3, 70,  4,
		2,
		0x0011,
	},
	{
		"Kobold",
		{ 1, -1, 0, 1, 0, -4 },
		-2, -3, -2, -1, 1, 8, 10, -8,
		9, 125,
		11,  3,
		60,  1, 130,  5,
		55,  1, 100,  5,
		3,
		0x0009,
	},
	{
		"Mind-Flayer",
		{ -3, 4, 4, 0, -2, -5 },
		10, 25, 15, 2, 5, 12, -10, -5,
		9, 140,
		100, 25,
		68,  6, 142, 15,
		63,  6, 112, 10,
		4,
		0x1746,
	},
	{
		"Nephilim",
		{  1,  2,  2,  2,  3,  2 },
		4,  5,  5,  2, 3, 13, 15, 10,
		10,  225,
		50, 50,
		82, 5, 190, 20,
		78,  6, 180, 15,
		0,
		0x1FFF,
	},
	{
		"Nibelung",
		{ 1, -1, 2, 0, 2, -4 },
		3, 5, 10, 1, 5, 10, 9, 0,
		11, 135,
		40, 12,
		43,  3, 92,  6,
		40,  3, 78,  3,
		5,
		0x1C0F,
	},
	{
		"Skeleton",
		{ 0, -2, -2, 0, 1, -4 },
		-5, -5, 5, -1, -1, 8, 10, 0,
		10, 145,
		100, 35,
		72,  6, 50, 5,
		66,  4, 50, 5,
		2,
		0x1F0F,
	},
	{
		"Spectre",
		{ -5, 4, 4, 2, -3, -6 },
		10, 25, 20, 5, 5, 14, -15, -5,
		7, 180,
		100, 30,
		72, 6, 100, 25,
		66, 4, 100, 20,
		5,
		0x1F4E,    /* Mage, Priest, Rogue, Warrior-Mage, Mystic */
	},
	{
		"Sprite",
		{  -4, 3, 3, 3, -2, 2 },
		10, 10, 10, 4, 10, 10, -12, 0,
		7, 175,
		50, 25,
		32,  2, 75,  2,
		29,  2, 65,  2,
		4,
		0x1E5E,
	},
	{
		"Vampire",
		{ 3, 3, -1, -1, 1, 2 },
		4, 10, 10, 4, 1, 8, 5, 0,
		11, 200,
		100, 30,
		72,  6, 180, 25,
		66,  4, 150, 20,
		5,
		0x1FFF,
	},
	{
		"Yeek",
		{ -2, 1, 1, 1, -2, -7 },
		2, 4, 10, 3, 5, 15, -5, -5,
		7, 100,
		14, 3,
		50,  3, 90,  6,
		50,  3, 75,  3,
		2,
		0x1E0F,
	},
	{
		"Zombie",
		{ 2, -6, -6, 1, 4, -5 },
		-5, -5, 8, -1, -1, 5, 15, 0,
		13, 135,
		100, 30,
		72, 6, 100, 25,
		66, 4, 100, 20,
		2,
		0x0001,
	}
};


/*
* Player Classes
*
*      Title,
*      {STR,INT,WIS,DEX,CON,CHA},
*      c_dis, c_dev, c_sav, c_stl, c_srh, c_fos, c_thn, c_thb,
*      x_dis, x_dev, x_sav, x_stl, x_srh, x_fos, x_thn, x_thb,
*      HD, Exp
*/
player_class class_info[MAX_CLASS] =
{
	{
		"Warrior",
		{ 5, -2, -2, 2, 2, -1},
		25, 18, 18, 1,  14, 2, 70, 55,
		12, 7,  10, 0,  0,  0,  45, 45,
		9,  0
	},
	{
		"Mage",
		{-5, 3, 0, 1, -2, 1},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 30
	},
	{
		"Priest",
		{-1, -3, 3, -1, 0, 2},
		25, 30, 32, 2,  16, 8, 48, 35,
		7,  10, 12, 0,  0,  0, 20, 20,
		2, 20
	},
	{
		"Rogue",
		{ 2, 1, -2, 3, 1, -1},
		45, 32, 28, 5, 32, 24, 60, 66,
		15, 10, 10, 0,  0,  0, 40, 30,
		6, 25
	},
	{
		"Ranger",
		{ 2, 2, 0, 1, 1, 1},
		30, 32, 28, 3,  24, 16, 56, 72,
		8,  10, 10, 0,  0,  0,  30, 45,
		4, 30
	},
	{
		"Paladin",
		{ 3, -3, 1, 0, 2, 2},
		20, 24, 26, 1,  12, 2, 68, 40,
		7,  10, 11, 0,  0,  0,  35, 30,
		6, 35
	},
	{
		"Warrior-Mage",
		{ 2, 2, 0, 1, 0, 1},
		30, 30, 28, 2,  18, 16, 50, 25,
		7,  10,  9, 0,  0,  0,  20, 20,
		4, 50
	},
	{
		"Diabolist",
		{ 2, 1, 0, 1, 2, -2},
		20, 25, 25, 1,  14, 12, 65, 40,
		7,  11, 10, 0,  0,  0,  34, 29,
		6, 35
	},
	{
		"Mystic",
		{ 2, -1, 1, 3, 2, 1},
		45, 32, 28, 5, 32, 24, 64, 60,
		15, 11, 10, 0,  0,  0, 40, 30,
		6, 40
	},
	{
		"Mindcrafter",
		{-1, 0, 3, -1, -1, 2},   /* note: spell stat is Wis */
		30, 30, 30, 3,  22, 16, 50, 40,
		10, 10, 10, 0,   0,  0, 20, 30,
		2, 25
	},
	{
		"High-Mage",
		{-5, 4, 0, 0, -2, 1},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 30
	},
	{
		"Druid",
		{-1, -3, 4, -2, 0, 3},
		25, 30, 32, 2,  16, 8, 48, 35,
		7,  10, 12, 0,  0,  0, 20, 20,
		2, 20
	},
	{
		"Demonologist",
		{-5, 4, 0, 0, -2, 1},
		30, 36, 30, 2,  16, 20, 34, 20,
		7,  13, 9,  0,  0,  0,  15, 15,
		0, 30
	},
};




/*
* Hack -- the spell information table.
*
*   Class Name
*
*   Spell Book
*   Spell Xtra
*
*   Spell Stat,
*   Spell Type,
*
*   Spell Level,
*   Spell Encumbrance,
*
*   Array of { Lev, Mana, Fail, Exp/Lev }
*/
player_magic magic_info[MAX_CLASS] =
{
{
/*** Warrior ***/

0,
0,

A_STR,
0,

99,
0,
{
{
/* Warrior: Life Magic */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Sorcery */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Nature */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Chaos */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Death */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Planar */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Folk */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
/* Warrior: Corporeal */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
},
},
{
/*** Mage ***/
TV_SORCERY_BOOK,
0,

A_INT,
0,

1,
300,
{
{
/* Mage: Life magic */
{ 1, 1, 30, 4 },
{ 3, 2, 35, 4 },
{ 4, 3, 35, 4 },
{ 5, 5, 35, 4 },
{ 7, 7, 35, 4 },
{ 9, 8, 40, 4 },
{ 12, 12, 40, 3 },
{ 15, 14, 45, 3 },

{ 16, 16, 45, 4},
{ 17, 17, 50, 4},
{ 18, 18, 50, 4},
{ 19, 19, 50, 4},
{ 20, 20, 50, 4},
{ 23, 23, 50, 4},
{ 30, 30, 55, 5},
{ 35, 70, 75, 5},

{ 26, 30, 50, 75 },
{ 28, 25, 70, 150 },
{ 33, 33, 60, 75 },
{ 35, 35, 60, 75 },
{ 35, 35, 70, 75 },
{ 35, 55, 80, 115 },
{ 39, 40, 80, 125 },
{ 46, 70, 80, 150 },

{ 9, 9, 50, 40 },
{ 25, 25, 50, 50 },
{ 35, 85, 80, 115 },
{ 42, 100, 80, 225 },
{ 45, 90, 80, 115 },
{ 48, 50, 80, 100 },
{ 49, 100, 80, 250 },
{ 50, 100, 80, 250 }
},
/* Mage: Sorcery */
{
{ 1, 1, 23, 4 },
{ 1, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 3, 3, 30, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 35, 5 },
{ 6, 5, 30, 4 },
{ 7, 7, 75, 9 },

{ 9, 7, 75, 8 },
{ 10, 7, 75, 8 },
{ 11, 7, 75, 7 },
{ 13, 7, 50, 6 },
{ 18, 12, 60, 8 },
{ 22, 12, 60, 8 },
{ 28, 20, 70, 15 },
{ 33, 30, 75, 20 },

{ 3, 3, 25, 15 },
{ 10, 10, 70, 40 },
{ 10, 10, 80, 40 },
{ 12, 12, 80, 40 },
{ 14, 10, 60, 25 },
{ 20, 18, 85, 50 },
{ 20, 18, 60, 25 },
{ 25, 25, 75, 19 },

{ 10, 10, 40, 20 },
{ 25, 25, 75, 70 },
{ 25, 30, 95, 160 },
{ 30, 40, 80, 120 },
{ 40, 80, 95, 200 },
{ 40, 100, 95, 200 },
{ 42, 50, 90, 175 },
{ 45, 70, 75, 250 },
},
/* Mage: Nature Magic */
{
{ 1, 1, 23, 4 },
{ 3, 3, 25, 3 },
{ 3, 3, 25, 1 },
{ 4, 4, 35, 4 },
{ 4, 4, 50, 5 },
{ 4, 5, 50, 5 },
{ 5, 5, 50, 5 },
{ 5, 5, 35, 4 },

{ 5, 5, 40, 6 },
{ 5, 5, 30, 6 },
{ 7, 6, 45, 6 },
{ 7, 6, 40, 6 },
{ 9, 6, 30, 5 },        
{ 19, 12, 55, 8 },
{ 25, 25, 90, 50 },
{ 40, 100, 95, 50 },

{ 7, 7, 20, 28 },
{ 9, 12, 40, 44 },
{ 10, 12, 75, 120 },
{ 15, 20, 85, 60 },
{ 30, 30, 90, 100 },
{ 37, 40, 90, 200 },
{ 38, 45, 75, 200},
{ 40, 90, 90, 250 },


{ 20, 18, 60, 25 },
{ 23, 23, 80, 50 },
{ 25, 25, 75, 29 },
{ 30, 27, 75, 35 },
{ 35, 30, 85, 65 },
{ 37, 35, 90, 100 },
{ 40, 90, 95, 250 },
{ 40, 75, 65, 150 }
},
/* Mage: Chaos Magic */
{
{ 1, 1, 20, 4 },
{ 1, 2, 22, 4 },
{ 2, 2, 25, 4 },
{ 5, 5, 30, 1 },
{ 9, 6, 50, 1 },
{ 13, 9, 45, 6 },
{ 14, 9, 45, 6 },
{ 15, 9, 35, 5 },

{ 17, 10, 25, 5 },
{ 19, 12, 45, 9 },
{ 21, 13, 45, 10 },
{ 23, 15, 50, 11 },
{ 25, 16, 50, 12 },
{ 25, 18, 60, 8 },
{ 30, 20, 80, 15 },
{ 35, 40, 85, 40 },

{ 11, 7, 45, 9 },
{ 15, 15, 80, 35 },
{ 16, 14, 80, 35 },
{25, 25, 85, 100 },
{ 30, 25, 85, 150 },
{ 42, 50, 85, 250 },
{ 45, 90, 80, 250 },
{ 47, 100, 90, 250 },

{ 20, 20, 66, 8 },
{ 35, 32, 85, 35 },
{ 37, 34, 75, 40 },
{ 41, 42, 85, 100 },
{ 43, 44, 80, 150 },
{ 45, 48, 85, 200 },
{ 47, 75, 80, 200 },
{ 49, 100, 85, 250 }
},
/* Mage: Death Magic */
{
{ 1, 1, 25, 4 },
{ 2, 2, 25, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 27, 3 },
{ 5, 5, 30, 4 },
{ 7, 10, 75, 6 },
{ 9, 9, 30, 4 },
{ 10, 10, 30, 4 },

{ 12, 12, 40, 5 },
{ 13, 12, 30, 4 },
{ 18, 15, 50, 10 },
{ 23, 20, 60, 16 },
{ 30, 75, 50, 30 },
{ 33, 35, 60, 16 },
{ 37, 25, 95, 25 },
{ 45, 50, 95, 150 },

{ 10, 20, 80, 180 },
{ 10, 15, 80, 30 },
{ 11, 11, 30, 15 },
{ 30, 25, 75, 50 },
{ 33, 35, 60, 125 },
{ 33, 90, 70, 90 },
{ 40, 40, 70, 200 },
{ 40, 75, 80, 100 },

{ 20, 20, 75, 50 },
{ 25, 66, 95 , 250 },
{ 30, 40, 95, 250 },
{ 33, 35, 70, 40 },
{ 37, 35, 80, 70 },
{ 42, 120, 95, 250 },
{ 45, 100, 90, 250 },
{ 47, 100, 90, 250 }
},
/* Mage: Planar Magic */
{
{ 1, 1, 50, 3 },
{ 3, 3, 50, 4 },
{ 5, 5, 75, 8 },
{ 6, 6, 80, 8 },
{ 7, 7, 40, 4 },
{ 9, 9, 60, 6 },
{ 14, 12, 60, 6 },
{ 17, 15, 60, 5 },

{ 20, 20, 80, 8 },
{ 24, 22, 60, 8 },
{ 28, 24, 60, 8 },
{ 30, 25, 70, 9 },
{ 33, 28, 80, 12 },
{ 35, 30, 70, 10 },
{ 40, 35, 80, 15 },
{ 42, 40, 70, 12 },

{ 15, 15, 80, 20 },
{ 24, 24, 70, 25 },
{ 26, 26, 70, 30 },
{ 30, 30,  70, 35 },
{ 35, 70, 80, 100 },
{ 40, 100, 90, 250 },
{ 42, 50, 50, 75 },
{ 45, 100, 90, 200 },

{ 30, 30, 60, 50 },
{ 35, 50, 90, 100 },
{ 36, 80, 80, 150 },
{ 39, 80, 80, 150 },
{ 42, 100, 80, 200 },
{ 47, 100, 80, 150 },
{ 48, 100, 80, 200 },
{ 49, 100, 80, 220 }
},
/* Mage: Folk Magic */
{
{ 1, 1, 20, 4 },
{ 1, 1, 33, 5 },
{ 1, 1, 33, 4 },
{ 2, 1, 33, 5 },
{ 2, 2, 33, 5 },
{ 4, 4, 40, 6 },
{ 5, 5, 33, 7 },
{ 6, 5, 44, 5 },

{ 7, 6, 40, 7 },
{ 8, 8, 60, 7 },
{ 9, 8, 50, 6 },
{ 9, 9, 50, 6 },
{ 9, 9, 50, 6 },
{ 11, 10, 50, 6 },
{ 12, 12, 50, 5 },
{ 13, 12, 50, 5 },

{ 14, 12, 50, 5 },
{ 15, 12, 50, 5 },
{ 16, 14, 33, 6 },
{ 18, 15, 50, 8 },
{ 20, 16, 60, 9 },
{ 23, 18, 60, 9 },
{ 25, 20, 70, 12 },
{ 25, 20, 60, 13 },

{ 28, 25, 70, 30 },
{ 35, 35, 80, 25 },
{ 38, 30, 60, 25 },
{ 40, 30, 70, 25 },
{ 41, 30, 66, 30 },
{ 42, 30, 80, 40 },
{ 45, 50, 70, 50 },
{ 49, 100, 80, 200 }
},
/* Mage: Corporeal */
{
{ 1, 1, 23, 4 },
{ 1, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 3, 3, 30, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 35, 5 },
{ 6, 5, 30, 4 },
{ 7, 7, 75, 9 },

{ 9, 7, 75, 8 },
{ 10, 7, 75, 8 },
{ 11, 7, 75, 7 },
{ 13, 7, 50, 6 },
{ 18, 12, 60, 8 },
{ 22, 12, 60, 8 },
{ 28, 20, 70, 15 },
{ 33, 30, 75, 20 },

{ 3, 3, 25, 15 },
{ 10, 10, 70, 40 },
{ 10, 10, 80, 40 },
{ 12, 12, 80, 40 },
{ 14, 10, 60, 25 },
{ 20, 18, 85, 50 },
{ 20, 18, 60, 25 },
{ 25, 25, 75, 19 },

{ 10, 10, 40, 20 },
{ 25, 25, 75, 70 },
{ 25, 30, 95, 160 },
{ 30, 40, 80, 120 },
{ 40, 80, 95, 200 },
{ 40, 100, 95, 200 },
{ 42, 50, 90, 175 },
{ 45, 70, 75, 250 },
},
}
},
{
/*** Priest ***/
TV_LIFE_BOOK,
0,

A_WIS,
1,

1,
350,
{
/* Priest: Life Magic */
{
{ 1, 1, 10, 4 },
{ 1, 2, 15, 4 },
{ 1, 2, 20, 4 },
{ 3, 2, 25, 1 },
{ 3, 3, 27, 2 },
{ 4, 4, 28, 2 },
{ 5, 4, 32, 4 },
{ 7, 5, 38, 4 },

{ 7, 6, 38, 5 },
{ 9, 6, 38, 4 },
{ 9, 7, 40, 4 },
{ 10, 8, 38, 4 },
{ 10, 8, 40, 4 },
{ 11, 8, 42, 4 },
{ 20, 16, 60, 7 },
{ 33, 55, 90, 15 },

{ 15, 14, 50, 50 },
{ 16, 14, 80, 60 },
{ 17, 14, 55, 70 },
{ 24, 20, 55, 70 },
{ 25, 20, 70, 120 },
{ 25, 25, 80, 250 },
{ 39, 32, 95, 200 },
{ 44, 44, 80, 250 },

{ 5, 5, 50, 80 },
{ 15, 14, 50, 100 },
{ 30, 50, 80, 130 },
{ 35, 70, 90, 250 },
{ 40, 50, 80, 130 },
{ 40, 40, 80, 200 },
{ 42, 90, 85, 200 },
{ 45, 90, 85, 250 },
},
/* Priest: Sorcery */
{
{ 2, 1, 23, 4 },
{ 3, 2, 24, 4 },
{ 4, 3, 25, 1 },
{ 5, 4, 30, 1 },
{ 6, 5, 30, 1 },
{ 7, 6, 35, 5 },
{ 9, 7, 30, 4 },
{ 11, 10, 75, 9 },

{ 13, 11, 75, 8 },
{ 14, 12, 75, 6 },
{ 15, 13, 75, 7 },
{ 16, 14, 50, 6 },
{ 22, 15, 60, 8 },
{ 27, 17, 65, 10 },
{ 30, 22, 70, 15 },
{ 36, 33, 75, 20 },

{ 7, 7, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 14, 80, 40 },
{ 15, 15, 80, 40 },
{ 18, 18, 60, 25 },
{ 20, 20, 85, 50 },
{ 22, 22, 60, 25 },
{ 27, 27, 75, 19 },

{ 13, 13, 40, 20 },
{ 24, 24, 75, 70 },
{ 27, 30, 95, 160 },
{ 33, 40, 80, 120 },
{ 42, 80, 95, 200 },
{ 42, 100, 95, 200 },
{ 45, 50, 90, 175 },
{ 48, 70, 75, 250 },
},
/* Priest: Nature Magic */
{
{ 2, 1, 25, 4 },
{ 5, 3, 25, 4 },
{ 5, 4, 25, 1 },
{ 6, 5, 35, 4 },
{ 6, 5, 50, 5 },
{ 6, 6, 50, 5 },
{ 7, 7, 50, 5 },
{ 7, 7, 35, 4 },

{ 7, 7, 40, 6 },
{ 8, 7, 30, 6 },
{ 9, 10, 40, 6 },
{ 10, 10, 40, 6 },
{ 11, 11, 30, 5 },
{ 20, 20, 65, 7 },
{ 30, 30, 55, 8 },
{ 42, 100, 95, 50 },

{ 9, 9, 20, 28 },
{ 11, 12, 40, 44 },
{ 12, 13, 75, 120 },
{ 18, 20, 85, 60 },
{ 35, 35, 80, 50 },
{ 39, 40, 90, 200 },
{ 40, 50, 85, 250 },
{ 42, 90, 90, 250 },

{ 22, 22, 60, 24 },
{ 25, 25, 60, 25 },
{ 27, 27, 75, 29 },
{ 32, 30, 75, 29 },
{ 37, 32, 85, 65 },
{ 39, 37, 90, 100 },
{ 42, 90, 95, 250 },
{ 44, 80, 65, 150 },
},
/* Priest: Chaos Magic */
{
{ 2, 1, 22, 4 },
{ 3, 2, 24, 4 },
{ 4, 3, 26, 4 },
{ 5, 4, 30, 6 },
{ 10, 6, 30, 5 },
{ 11, 6, 50, 5 },
{ 16, 11, 50, 6 },
{ 17, 11, 35, 5 },

{ 19, 15, 50, 7 }, 
{ 21, 16, 50, 9 },
{ 23, 18, 80, 20 },
{ 25, 18, 50, 11 },
{ 27, 20, 65, 12 },
{ 29, 22, 60, 8 },
{ 33, 23, 80, 15},
{ 37, 42, 85, 40 },

{ 14, 11, 45, 9 },
{ 17, 17, 70, 20 },
{ 20, 18, 80, 35 },
{ 27, 27, 70, 35 },
{ 35, 30, 85, 150 },
{ 45, 55, 95, 250 },
{ 47, 90, 90, 250 },
{ 49, 100, 90, 250 },

{ 25, 25, 66, 8 },
{ 37, 37, 85, 35 },
{ 39, 37, 75, 50 },
{ 43, 45, 85, 100 },
{ 45, 47, 90, 150 },
{ 47, 50, 95, 200 },
{ 49, 95, 80, 200 },
{ 50, 100, 95, 250 },
},
/* Priest: Death Magic */
{
{ 1, 1, 25, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 25, 4 },
{ 5, 4, 27, 4 },
{ 7, 7, 30, 4 },
{ 9, 11, 75, 6 },
{ 11, 11, 30, 4 },
{ 12, 12, 40, 5 },

{ 14, 14, 40, 5 },
{ 16, 16, 30, 4 },
{ 21, 20, 50, 10 },
{ 25, 24, 60, 16 },
{ 33, 75, 90, 30 },
{ 35, 35, 60, 16 },
{ 40, 30, 95, 100 },
{ 50, 52, 95, 150 },

{ 13, 20, 80, 180 },
{ 13, 15, 80, 30 },
{ 14, 15, 30, 15 },   
{ 33, 33, 70, 33 },
{ 35, 35, 60, 125 },
{ 35, 95, 70, 90 },
{ 44, 44, 80, 200 },
{ 45, 75, 80, 100 },

{ 25, 25, 75, 50 },
{ 30, 75, 95, 250 },
{ 35, 45, 95, 250 },
{ 40, 40, 70, 40 },
{ 42, 40, 80, 70 },
{ 48, 125, 95, 250 },
{ 49, 100, 90, 250 },
{ 50, 111, 90, 250 },
},
/* Priest: Planar Magic */
{
{ 1, 1, 50, 3 },
{ 4, 4, 50, 4 },
{ 6, 5, 75, 8 },
{ 7, 7, 80, 8 },
{ 9, 9, 40, 4 },
{ 11, 11, 60, 6 },
{ 17, 14, 60, 6 },
{ 19, 17, 60, 5 },

{ 22, 22, 80, 8 },
{ 26, 24, 60, 8 },
{ 30, 25, 60, 8 },
{ 32, 30, 70, 9 },
{ 35, 30, 80, 12 },
{ 38, 35, 70, 10 },
{ 42, 40, 80, 15 },
{ 45, 45, 70, 12 },

{ 17, 17, 80, 20 },
{ 27, 25, 70, 25 },
{ 29, 27, 70, 30 },
{ 33, 30,  70, 35 },
{ 38, 75, 80, 100 },
{ 41, 110, 90, 250 },
{ 45, 55, 50, 75 },
{ 49, 125, 90, 200 },

{ 32, 30, 60, 50 },
{ 38, 55, 90, 100 },
{ 40, 85, 80, 150 },
{ 43, 85, 80, 150 },
{ 46, 110, 80, 200 },
{ 48, 115, 80, 150 },
{ 49, 120, 80, 200 },
{ 50, 125, 80, 220 }
},
/* Priest: Folk Magic */
{
{ 1, 1, 20, 4 },
{ 1, 1, 33, 5 },
{ 2, 1, 33, 4 },
{ 2, 2, 33, 5 },
{ 3, 3, 33, 5 },
{ 5, 5, 40, 6 },
{ 6, 6, 33, 7 },
{ 7, 6, 44, 5 },

{ 8, 7, 40, 7 },
{ 9, 8, 60, 7 },
{ 10, 9, 50, 6 },
{ 11, 10, 50, 6 },
{ 12, 11, 50, 6 },
{ 13, 12, 50, 6 },
{ 14, 13, 50, 5 },
{ 15, 14, 50, 5 },

{ 16, 15, 50, 5 },
{ 17, 16, 50, 5 },
{ 18, 17, 33, 6 },
{ 19, 18, 50, 8 },
{ 22, 20, 60, 9 },
{ 24, 22, 60, 9 },
{ 27, 24, 70, 12 },
{ 29, 26, 60, 13 },

{ 33, 30, 80, 50 },
{ 37, 36, 80, 25 },
{ 40, 37, 60, 25 },
{ 42, 38, 70, 25 },
{ 44, 39, 66, 30 },
{ 46, 40, 80, 40 },
{ 47, 55, 70, 50 },
{ 50, 120, 80, 200 }
},
/* Priest: Corporeal */
{
{ 2, 1, 23, 4 },
{ 3, 2, 24, 4 },
{ 4, 3, 25, 1 },
{ 5, 4, 30, 1 },
{ 6, 5, 30, 1 },
{ 7, 6, 35, 5 },
{ 9, 7, 30, 4 },
{ 11, 10, 75, 9 },

{ 13, 11, 75, 8 },
{ 14, 12, 75, 6 },
{ 15, 13, 75, 7 },
{ 16, 14, 50, 6 },
{ 22, 15, 60, 8 },
{ 27, 17, 65, 10 },
{ 30, 22, 70, 15 },
{ 36, 33, 75, 20 },

{ 7, 7, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 14, 80, 40 },
{ 15, 15, 80, 40 },
{ 18, 18, 60, 25 },
{ 20, 20, 85, 50 },
{ 22, 22, 60, 25 },
{ 27, 27, 75, 19 },

{ 13, 13, 40, 20 },
{ 24, 24, 75, 70 },
{ 27, 30, 95, 160 },
{ 33, 40, 80, 120 },
{ 42, 80, 95, 200 },
{ 42, 100, 95, 200 },
{ 45, 50, 90, 175 },
{ 48, 70, 75, 250 },
},
}
},
{
/*** Rogue ***/
TV_SORCERY_BOOK,
0,

A_INT,
0,

5,
350,

{
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
/* Rogue (Burglar): Sorcery */
{
{ 5, 1, 50, 1 },
{ 7, 2, 55, 1 },
{ 8, 3, 65, 1 },
{ 9, 3, 65, 1 },
{ 13, 6, 75, 1 },
{ 15, 7, 75, 1 },
{ 17, 9, 75, 1 },
{ 21, 12, 80, 1 },

{ 25, 14, 80, 1 },
{ 27, 15, 80, 1 },
{ 29, 17, 75, 2 },
{ 30, 20, 80, 4 },
{ 31, 23, 80, 5 },
{ 32, 25, 70, 6 },
{ 35, 30, 80, 12 },
{ 40, 35, 75, 20 },

{ 9, 3, 65, 5 },
{ 13, 10, 70, 5 },
{ 14, 10, 80, 8 },
{ 15, 10, 80, 8 },
{ 16, 10, 60, 10 },
{ 17, 20, 80, 20 },
{ 18, 17, 60, 30 },
{ 30, 35, 75, 15 },

{ 15, 15, 40, 10 },
{ 20, 20, 70, 50 },
{ 35, 40, 95, 100 },
{ 37, 40, 80, 100 },
{ 43, 80, 80, 100 },
{ 44, 100, 80, 100 },
{ 45, 50, 70, 100 },
{ 99, 0, 0, 0 },
},
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
/* Rogue (Assassin): Death Magic */
{
{ 5, 3, 40, 1 },
{ 7, 4, 40, 1 },
{ 9, 5, 50, 1 },
{ 13, 7, 60, 1 },
{ 15, 7, 80, 1 },
{ 17, 15, 80, 1 },
{ 19, 17, 30, 1 },
{ 19, 19, 30, 1 },

{ 21, 21, 60, 3 },
{ 23, 23, 75, 4 },
{ 27, 25, 75, 4 },
{ 30, 30, 75, 4 },
{ 35, 35, 75, 5 },
{ 45, 45, 60, 10 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },

{ 20, 25, 80, 100 },
{ 23, 20, 40, 20 },
{ 28, 28, 75, 25 },
{ 32, 32, 80, 50 },
{ 46, 45, 75, 40 },
{ 48, 100, 90, 100 },
{ 50, 50, 75, 50 },
{ 99, 0, 0, 0 },


{ 30, 30, 80, 50 },
{ 31, 80, 80, 250 },
{ 32, 40, 90, 150 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 50, 125, 90, 250 },
},
/* Rogue (Card Sharp): Planar Magic */
{
{ 5, 2, 50, 3 },
{ 7, 5, 50, 4 },
{ 9, 7, 75, 8 },
{ 11, 9, 80, 8 },
{ 13, 11, 40, 4 },
{ 15, 13, 60, 6 },
{ 19, 15, 60, 6 },
{ 21, 20, 60, 5 },

{ 25, 22, 80, 8 },
{ 30, 26, 60, 8 },
{ 33, 26, 60, 8 },
{ 35, 32, 70, 9 },
{ 40, 35, 80, 12 },
{ 42, 38, 70, 10 },
{ 46, 44, 80, 15 },
{ 49, 50, 70, 12 },

{ 20, 15, 80, 20 },
{ 30, 30, 70, 25 },
{ 33, 30, 70, 30 },
{ 38, 33,  70, 35 },
{ 42, 90, 80, 100 },
{ 45, 150, 90, 250 },
{ 48, 75, 50, 75 },
{ 99, 0, 0, 0 },

{ 35, 30, 60, 50 },
{ 42, 65, 90, 100 },
{ 44, 100, 80, 150 },
{ 46, 100, 80, 150 },
{ 99, 0, 0, 0 },
{ 49, 125, 80, 150 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
},
/* Rogue (Thief): Folk Magic */
{
{ 1, 1, 20, 4 },
{ 1, 1, 33, 5 },
{ 2, 1, 33, 4 },
{ 2, 2, 33, 5 },
{ 3, 3, 33, 5 },
{ 5, 5, 40, 6 },
{ 6, 6, 33, 7 },
{ 7, 7, 44, 5 },

{ 8, 8, 40, 7 },
{ 9, 9, 60, 7 },
{ 10, 10, 50, 6 },
{ 11, 11, 50, 6 },
{ 13, 11, 50, 6 },
{ 14, 12, 50, 6 },
{ 15, 13, 50, 5 },
{ 16, 14, 50, 5 },

{ 17, 15, 50, 5 },
{ 18, 16, 50, 5 },
{ 19, 17, 33, 6 },
{ 20, 20, 50, 8 },
{ 23, 22, 60, 9 },
{ 25, 24, 60, 9 },
{ 28, 25, 70, 12 },
{ 30, 28, 60, 13 },

{ 35, 30, 80, 40 },
{ 39, 36, 80, 25 },
{ 42, 37, 60, 25 },
{ 44, 38, 70, 25 },
{ 46, 40, 66, 30 },
{ 47, 42, 80, 40 },
{ 48, 60, 70, 50 },
{ 50, 125, 80, 200 }
},
/* Rogue: Corporeal Magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
}
},
{
/*** Ranger ***/
TV_LIFE_BOOK,
0,

A_INT,
0,

3,
400,

{
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
/* Ranger: Sorcery */
{
{ 3, 2, 35, 2 },
{ 3, 3, 35, 2 },
{ 5, 4, 40, 2 },
{ 7, 4, 40, 2 },
{ 9, 6, 40, 2 },
{ 11, 8, 45, 2 },
{ 13, 8, 40, 3 },
{ 17,17, 90, 4 },

{ 20, 19, 85, 4 },
{ 23, 25, 90, 3 },
{ 25, 25, 60, 3 },
{ 27, 25, 85, 3 },
{ 31, 27, 70, 3 },
{ 34, 35, 70, 4 },
{ 38, 37, 70, 8 },
{ 42, 40, 90, 10 },

{ 15, 7, 75, 20 },
{ 15, 20, 70, 25 },
{ 17, 17, 70, 25 },
{ 18, 18, 80, 25 },
{ 19, 25, 65, 20 },
{ 23, 25, 60, 20 },
{ 27, 27, 60, 15 },
{ 35, 35, 75, 13 },

{ 20, 20, 45, 5 },
{ 27, 27, 70, 50 },
{ 37, 60, 95, 120 },
{ 40, 40, 95, 120 },
{ 45, 80, 95, 200 },
{ 45, 100, 95, 200 },
{ 50, 50, 90, 175 },
{ 99, 0, 0, 0 },
},
/* Ranger: Nature Magic */
{
{ 3, 1, 35, 2 },
{ 4, 3, 40, 2 },
{ 4, 4, 40, 3 },
{ 5, 7, 55, 2 },
{ 6, 7, 50, 3 },
{ 7, 7, 50, 3 },
{ 8, 7, 50, 3 },
{ 9, 7, 45, 3 },

{ 9, 7, 80, 4 },
{ 10, 7, 40, 3 },
{ 11, 9, 40, 4 },
{ 12, 9, 55, 4 },
{ 14, 9, 55, 4 },
{ 18, 20, 65, 8 },
{ 23, 23, 65, 10 },
{ 40, 100, 95, 50 },

{ 10, 10, 50, 25 },
{ 12, 12, 50, 25 },
{ 14, 15, 70, 60 },
{ 20, 30, 85, 70 },
{ 35, 30, 80, 75 },
{ 38, 40, 90, 200 },
{ 40, 55, 90, 250 },
{ 42, 80, 90, 250 },

{ 25, 28, 60, 150 },
{ 26, 26, 60, 100 },
{ 30, 35, 75, 125 },
{ 32, 29, 75, 35 },
{ 36, 33, 75, 45 },
{ 40, 35, 75, 100 },
{ 41, 80, 95, 250 },
{ 42, 80, 75, 150 },
},
/* Ranger: Chaos Magic */
{
{ 3, 1, 20, 1 },
{ 3, 3, 35, 2 },
{ 5, 3, 35, 2 },
{ 7, 5, 45, 2 },
{ 14, 12, 40, 2 },
{ 20, 16, 50, 6 },
{ 25, 21, 60, 3 },
{ 25, 22, 60, 3 },

{ 27, 23, 60, 5 },
{ 30, 25, 60, 8 },
{ 33, 30, 70, 13 },
{ 35, 31, 70, 10 },
{ 37, 35, 75, 15 },
{ 39, 29, 65, 5 },
{ 43, 30, 95, 15 },
{ 48, 50, 85, 30 },

{ 22, 20, 60, 30 },
{ 25, 25, 70, 20 },
{ 28, 25, 80, 45 },
{ 35, 32, 70, 35 },
{ 38, 35, 85, 150 },
{ 42, 75, 95, 250 },
{ 48, 100, 90, 250 },
{ 99, 0, 0, 0 },

{ 33, 33, 66, 8 },
{ 40, 45, 85, 35 },
{ 42, 42, 75, 42 },
{ 48, 48, 85, 100 },
{ 50, 50, 90, 150 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
},
/* Ranger: Death Magic */
{
{ 5, 2, 40, 3 },
{ 5, 3, 40, 3 },
{ 7, 4, 50, 3 },
{ 9, 5, 40, 3 },
{ 11, 8, 40, 3 },
{ 17, 25, 75, 4 },
{ 19, 19, 50, 3 },
{ 22, 22, 50, 3 },

{ 24, 24, 55, 3 },
{ 26, 26, 50, 3 },
{ 28, 28, 75, 4 },
{ 30, 30, 80, 5 },
{ 40, 80, 95, 20 },
{ 45, 40, 60, 9 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },

{ 25, 30, 80, 125 },
{ 25, 25, 80, 100 },
{ 27, 27, 40, 40 },
{ 39, 39, 76, 50 },
{ 45, 45, 80, 100 },
{ 46, 100, 90, 100 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },

{ 35, 35, 75, 50 },
{ 38, 90, 80, 100 },
{ 40, 45, 95, 200 },
{ 48, 50, 30, 75 },
{ 50, 50, 90, 75 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 }
},
/* Ranger: Planar Magic */
{
{ 3, 1, 50, 3 },
{ 6, 6, 50, 4 },
{ 9, 7, 75, 8 },
{ 10, 8, 80, 8 },
{ 13, 10, 40, 4 },
{ 17, 15, 60, 6 },
{ 20, 17, 60, 6 },
{ 22, 20, 60, 5 },

{ 24, 22, 80, 8 },
{ 28, 25, 60, 8 },
{ 33, 26, 60, 8 },
{ 36, 32, 70, 9 },
{ 38, 33, 80, 12 },
{ 42, 38, 70, 10 },
{ 45, 42, 80, 15 },
{ 99, 0, 0, 0 },

{ 20, 20, 80, 20 },
{ 28, 26, 70, 25 },
{ 31, 30, 70, 30 },
{ 36, 33,  70, 35 },
{ 41, 80, 80, 100 },
{ 44, 120, 90, 250 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },

{ 35, 33, 60, 50 },
{ 40, 65, 90, 100 },
{ 99, 0, 0, 0 },
{ 47, 95, 80, 150 },
{ 50, 120, 80, 200 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
{ 99, 0, 0, 0 },
},
/* Ranger: Folk Magic */
{
{ 3, 2, 20, 4 },
{ 3, 2, 33, 5 },
{ 4, 3, 33, 4 },
{ 4, 3, 33, 5 },
{ 5, 4, 33, 5 },
{ 6, 6, 40, 6 },
{ 7, 7, 33, 7 },
{ 8, 8, 44, 5 },

{ 9, 9, 40, 7 },
{ 10, 10, 60, 7 },
{ 11, 11, 50, 6 },
{ 12, 12, 50, 6 },
{ 14, 13, 50, 6 },
{ 15, 14, 50, 6 },
{ 16, 15, 50, 5 },
{ 17, 16, 50, 5 },

{ 18, 17, 50, 5 },
{ 19, 18, 50, 5 },
{ 20, 19, 33, 6 },
{ 22, 20, 50, 8 },
{ 25, 23, 60, 9 },
{ 27, 26, 60, 9 },
{ 29, 27, 70, 12 },
{ 33, 30, 60, 13 },

{ 38, 36, 80, 40 },
{ 42, 38, 80, 25 },
{ 44, 38, 60, 25 },
{ 46, 40, 70, 25 },
{ 47, 42, 66, 30 },
{ 48, 44, 80, 40 },
{ 49, 65, 70, 50 },
{ 99, 0, 0, 0 }
},
/* Ranger: Corporeal */
{
{ 3, 2, 35, 2 },
{ 3, 3, 35, 2 },
{ 5, 4, 40, 2 },
{ 7, 4, 40, 2 },
{ 9, 6, 40, 2 },
{ 11, 8, 45, 2 },
{ 13, 8, 40, 3 },
{ 17,17, 90, 4 },

{ 20, 19, 85, 4 },
{ 23, 25, 90, 3 },
{ 25, 25, 60, 3 },
{ 27, 25, 85, 3 },
{ 31, 27, 70, 3 },
{ 34, 35, 70, 4 },
{ 38, 37, 70, 8 },
{ 42, 40, 90, 10 },

{ 15, 7, 75, 20 },
{ 15, 20, 70, 25 },
{ 17, 17, 70, 25 },
{ 18, 18, 80, 25 },
{ 19, 25, 65, 20 },
{ 23, 25, 60, 20 },
{ 27, 27, 60, 15 },
{ 35, 35, 75, 13 },

{ 20, 20, 45, 5 },
{ 27, 27, 70, 50 },
{ 37, 60, 95, 120 },
{ 40, 40, 95, 120 },
{ 45, 80, 95, 200 },
{ 45, 100, 95, 200 },
{ 50, 50, 90, 175 },
{ 99, 0, 0, 0 },
},
}
},

{
/*** Paladin ***/

TV_LIFE_BOOK,
0,

A_WIS,
1,

1,
400,

{
{
/* Paladin: Life Magic */
{ 1, 1, 30, 4 },
{ 2, 2, 35, 4 },
{ 3, 3, 35, 4 },
{ 4, 3, 35, 4 },
{ 5, 4, 35, 4 },
{ 8, 5, 40, 4 },
{ 11, 9, 40, 3 },
{ 13, 10, 45, 3 },

{ 14, 11, 45, 4},
{ 15, 15, 50, 4},
{ 17, 15, 50, 4},
{ 18, 15, 50, 4},
{ 18, 15, 50, 4},
{ 19, 15, 50, 4},
{ 30, 25, 55, 5},
{ 35, 70, 75, 5},

{ 25, 22, 50, 75 },
{ 28, 24, 70, 150 },
{ 30, 25, 60, 75 },
{ 33, 30, 60, 75 },
{ 35, 32, 70, 75 },
{ 35, 55, 80, 115 },
{ 39, 38, 80, 125 },
{ 46, 60, 80, 150 },

{ 9, 9, 50, 40 },
{ 25, 20, 50, 50 },
{ 35, 65, 80, 115 },
{ 40, 80, 80, 225 },
{ 45, 80, 80, 115 },
{ 45, 45, 80, 100 },
{48, 100, 90, 250 },
{ 50, 100, 80, 250 }
},
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Paladin: Death Magic */
{
{ 1, 1, 25, 4 },
{ 3, 2, 25, 4 },
{ 4, 3, 25, 4 },
{ 6, 5, 27, 4 },
{ 8, 8, 30, 4 },
{ 10, 11, 75, 6 },
{ 12, 12, 30, 4 },
{ 15, 15, 40, 5 },

{ 17, 17, 40, 5 },
{ 19, 19, 30, 4 },
{ 23, 23, 50, 10 },
{ 28, 26, 60, 16 },
{ 35, 75, 90, 30 },
{ 40, 35, 60, 16 },
{ 45, 35, 95, 100 },
{ 50, 52, 95, 150 },

{ 15, 20, 80, 180 },
{ 15, 20, 80, 30 },
{ 18, 20, 30, 15 },
{ 38, 38, 70, 50 },
{ 40, 40, 60, 125 },
{ 42, 100, 70, 100 },
{ 48, 50, 80, 200 },
{ 48, 75, 80, 100 },


{ 30, 35, 75, 50 },
{ 36, 85, 90, 200 },
{ 38, 45, 95, 250 },
{ 45, 45, 70, 40 },
{ 47, 45, 80, 70 },
{ 50, 150, 95, 250 },
{ 50, 100, 95, 250 },
{ 50, 111, 95, 250 }
},
/* Paladin: No Planar magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
/* Paladin: No Folk Magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                       			
},
{
/* Paladin: No Corporeal Magic */
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
}
},
{
/*** Warrior-Mage ***/
TV_SORCERY_BOOK,
0,

A_INT,
0,

1,
350,

{
{
{ 2, 2, 30, 4 },
{ 4, 4, 35, 4 },
{ 5, 5, 35, 4 },
{ 6, 6, 35, 4 },
{ 8, 8, 35, 4 },
{ 9, 9, 40, 4 },
{ 14, 14, 40, 3 },
{ 16, 16, 45, 3 },

{ 18, 18, 45, 4},
{ 20, 20, 50, 4},
{ 22, 22, 50, 4},
{ 24, 24, 50, 4},
{ 26, 26, 50, 4},
{ 28, 28, 50, 4},
{ 33, 33, 55, 5},
{ 40, 70, 75, 5},

{ 28, 28, 50, 75 },
{ 30, 30, 70, 150 },
{ 34, 34, 60, 75 },
{ 36, 36, 60, 75 },
{ 38, 38, 70, 75 },
{ 42, 55, 80, 115 },
{ 45, 45, 80, 125 },
{ 50, 70, 80, 150 },

{ 10, 10, 50, 40 },
{ 28, 28, 50, 50 },
{ 38, 85, 80, 115 },
{ 45, 90, 80, 225 },
{ 46, 90, 80, 115 },
{ 48, 50, 80, 100 },
{ 49, 100, 90, 100 },
{ 50, 100, 80, 250 }
},
/* Warrior-Mage: Sorcery */
{
{ 1, 1, 23, 4 },
{ 2, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 30, 1 },
{ 6, 6, 35, 5 },
{ 7, 7, 30, 4 },
{ 8, 8, 75, 9 },

{ 10, 9, 75, 8 },
{ 11, 10, 75, 8 },
{ 12, 11, 75, 7 },
{ 13, 12, 50, 6 },
{ 20, 15, 60, 8 },
{ 27, 18, 60, 8 },
{ 33, 25, 70, 15 },
{ 40, 40, 75, 20 },

{ 4, 4, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 12, 80, 40 },
{ 15, 12, 70, 30 },
{ 16, 14, 60, 25 },
{ 19, 19, 85, 50 },
{ 24, 22, 60, 25 },
{ 28, 28, 75, 19 },

{ 12, 12, 40, 20 },
{ 19, 19, 75, 70 },
{ 30, 35, 95, 160 },
{ 35, 45, 80, 120 },
{ 42, 85, 95, 200 },
{ 45, 100, 95, 200 },
{ 46, 55, 90, 175 },
{ 48, 75, 75, 250 },
},
/* Warrior-Mage: Nature Magic */
{
{ 2, 2, 23, 4 },
{ 3, 3, 25, 3 },
{ 4, 4, 25, 1 },
{ 5, 5, 35, 4 },
{ 6, 6, 50, 5 },
{ 7, 7, 50, 5 },
{ 8, 8, 50, 5 },
{ 9, 9, 35, 4 },

{ 10, 10, 40, 6 },
{ 11, 11, 30, 6 },
{ 12, 12, 45, 6 },
{ 13, 13, 40, 6 },
{ 14, 14, 30, 5 },
{ 19, 15, 65, 7 },
{ 31, 31, 65, 10 },
{ 45, 100, 95, 50 },

{ 9, 9, 20, 28 },
{ 12, 12, 40, 44 },
{ 15, 15, 75, 120 },
{ 20, 22, 85, 60 },
{ 38, 38, 85, 80 },
{ 40, 42, 90, 200 },
{ 45, 48, 75, 200},
{ 49, 95, 90, 250 },


{ 25, 25, 60, 25 },
{ 27, 27, 60, 25 },
{ 28, 28, 75, 29 },
{ 33, 33, 75, 35 },
{ 38, 38, 85, 65 },
{ 41, 41, 90, 100 },
{ 45, 95, 95, 250 },
{ 50, 85, 65, 150 },
},
/* Warrior-Mage: Chaos Magic */
{
{ 2, 2, 20, 4 },
{ 3, 3, 22, 4 },
{ 4, 4, 25, 4 },
{ 5, 5, 30, 6 },
{ 8, 8, 30, 1 },
{ 11, 11, 45, 5 },
{ 17, 15, 45, 6 },
{ 18, 17, 35, 5 },

{ 21, 21, 45, 7 },
{ 23, 22, 45, 9 },
{ 27, 25, 50, 20 },
{ 29, 30, 50, 11 },
{ 33, 33, 50, 12 },
{ 37, 35, 60, 8 },
{ 41, 40, 80, 15 },
{ 48, 50, 85, 40 },

{ 12, 12, 45, 9 },
{ 17, 16, 60, 20 },
{ 20, 18, 80, 35 },
{ 27, 25, 60, 35 },
{ 35, 30, 85, 150 },
{ 45, 55, 85, 250 },
{ 49, 95, 80, 250 },
{ 50, 111, 80, 250 },

{ 24, 20, 66, 8 },
{ 40, 35, 85, 35 },
{ 42, 40, 75, 40 },
{ 46, 44, 85, 100 },
{ 48, 48, 80, 150 },
{ 49, 50, 85, 200 },
{ 50, 100, 80, 250 },
{ 50, 100, 85, 250 },
},
/* Warrior-Mage: Death Magic */
{
{ 1, 1, 25, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 25, 4 },
{ 4, 4, 27, 3 },
{ 7, 7, 30, 4 },
{ 9, 10, 75, 6 },
{ 10, 10, 30, 4 },
{ 12, 12, 30, 4 },

{ 14, 14, 40, 5 },
{ 16, 16, 30, 4 },
{ 21, 21, 50, 10 },
{ 25, 25, 60, 16 },
{ 35, 75, 50, 30 },
{ 40, 40, 60, 16 },
{ 44, 45, 95, 25 },
{ 48, 55, 95, 150 },

{ 10, 22, 80, 180 },
{ 12, 18, 80, 30 },
{ 14, 18, 30, 15 },
{ 30, 30, 75, 50 },
{ 40, 40, 60, 125 },
{ 42, 90, 70, 90 },
{ 45, 50, 70, 200 },
{ 48, 85, 80, 100 },

{ 24, 24, 75, 50 },
{ 33, 80, 75, 150 },
{ 35, 45, 95, 250 },
{ 42, 50, 70, 40 },
{ 45, 55, 80, 70 },
{ 50, 135, 95, 250 },
{ 50, 100, 95, 250 },
{ 50, 123, 95, 250 },
},
/* Warrior-Mage: Planar Magic */
{
{ 1, 1, 50, 3 },
{ 5, 5, 50, 4 },
{ 7, 7, 75, 8 },
{ 8, 7, 80, 8 },
{ 10, 10, 40, 4 },
{ 12, 12, 60, 6 },
{ 18, 15, 60, 6 },
{ 20, 18, 60, 5 },

{ 24, 23, 80, 8 },
{ 28, 25, 60, 8 },
{ 31, 26, 60, 8 },
{ 33, 30, 70, 9 },
{ 38, 32, 80, 12 },
{ 40, 38, 70, 10 },
{ 44, 42, 80, 15 },
{ 48, 46, 70, 12 },

{ 19, 18, 80, 20 },
{ 29, 27, 70, 25 },
{ 31, 30, 70, 30 },
{ 35, 33,  70, 35 },
{ 40, 80, 80, 100 },
{ 42, 120, 90, 250 },
{ 46, 55, 50, 75 },
{ 50, 135, 90, 200 },

{ 33, 30, 60, 50 },
{ 40, 60, 90, 100 },
{ 42, 95, 80, 150 },
{ 45, 95, 80, 150 },
{ 46, 120, 80, 200 },
{ 48, 125, 80, 150 },
{ 49, 130, 80, 200 },
{ 50, 135, 80, 220 }
},
/* Warrior-Mage: Folk Magic */
{
{ 1, 1, 20, 4 },
{ 2, 1, 33, 5 },
{ 2, 2, 33, 4 },
{ 3, 3, 33, 5 },
{ 4, 4, 33, 5 },
{ 5, 5, 40, 6 },
{ 6, 6, 33, 7 },
{ 7, 7, 44, 5 },

{ 8, 8, 40, 7 },
{ 9, 9, 60, 7 },
{ 11, 10, 50, 6 },
{ 12, 11, 50, 6 },
{ 13, 12, 50, 6 },
{ 14, 13, 50, 6 },
{ 15, 14, 50, 5 },
{ 16, 15, 50, 5 },

{ 17, 16, 50, 5 },
{ 18, 17, 50, 5 },
{ 19, 18, 33, 6 },
{ 20, 20, 50, 8 },
{ 23, 22, 60, 9 },
{ 25, 23, 60, 9 },
{ 29, 25, 70, 12 },
{ 30, 27, 60, 13 },

{ 35, 30, 80, 50 },
{ 39, 38, 80, 25 },
{ 41, 40, 60, 25 },
{ 43, 42, 70, 25 },
{ 45, 44, 66, 30 },
{ 47, 45, 80, 40 },
{ 48, 65, 70, 50 },
{ 50, 140, 80, 200 }
},
/* Warrior-Mage: Corporeal */
{
{ 1, 1, 23, 4 },
{ 2, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 30, 1 },
{ 6, 6, 35, 5 },
{ 7, 7, 30, 4 },
{ 8, 8, 75, 9 },

{ 10, 9, 75, 8 },
{ 11, 10, 75, 8 },
{ 12, 11, 75, 7 },
{ 13, 12, 50, 6 },
{ 20, 15, 60, 8 },
{ 27, 18, 60, 8 },
{ 33, 25, 70, 15 },
{ 40, 40, 75, 20 },

{ 4, 4, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 12, 80, 40 },
{ 15, 12, 70, 30 },
{ 16, 14, 60, 25 },
{ 19, 19, 85, 50 },
{ 24, 22, 60, 25 },
{ 28, 28, 75, 19 },

{ 12, 12, 40, 20 },
{ 19, 19, 75, 70 },
{ 30, 35, 95, 160 },
{ 35, 45, 80, 120 },
{ 42, 85, 95, 200 },
{ 45, 100, 95, 200 },
{ 46, 55, 90, 175 },
{ 48, 75, 75, 250 },
},
}
},

{
/*** Diabolist ***/
TV_LIFE_BOOK,
0,

A_INT,
1,

2,
400,

{
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},

{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Diabolist: Chaos Magic */
{
{ 2, 1, 20, 4 },
{ 3, 2, 22, 4 },
{ 4, 3, 25, 4 },
{ 5, 4, 30, 6 },
{ 7, 7, 30, 1 },
{ 8, 7, 45, 5 },
{ 15, 9, 45, 6 },
{ 16, 10, 35, 5 },

{ 19, 12, 45, 7 },
{ 22, 14, 45, 9 },
{ 25, 17, 50, 20 },
{ 28, 18, 50, 11 },
{ 30, 20, 50, 12 },
{ 33, 24, 60, 8 },
{ 36, 26, 80, 15 },
{ 40, 45, 85, 40 },

{ 11, 11, 45, 9 },
{ 14, 14, 60, 20 },
{ 16, 15, 80, 35 },
{ 23, 23, 60, 35 },
{ 30, 30, 85, 150 },
{ 42, 50, 85, 250 },
{ 45, 90, 80, 250 },
{ 47, 100, 80, 250 },

{ 23, 23, 66, 10},
{ 35, 35, 85, 35 },
{ 37, 37, 75, 40 },
{ 41, 42, 85, 100 },
{ 43, 44, 80, 150 },
{ 45, 48, 85, 200 },
{  48, 100, 80, 220 },
{ 49, 100, 85, 250 },
},

{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Diabolist: No Planar magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        
},
/* Diabolist: No Folk Magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        				
},

{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
}
},
{
/*** Mystic ***/

TV_LIFE_BOOK,
0,

A_WIS,
0,

1,
300,

{
{
{ 1, 1, 30, 4 },
{ 2, 2, 35, 4 },
{ 3, 3, 35, 4 },
{ 4, 4, 35, 4 },
{ 5, 5, 35, 4 },
{ 8, 6, 40, 4 },
{ 11, 10, 40, 3 },
{ 13, 12, 45, 3 },

{ 15, 12, 45, 4},
{ 16, 15, 50, 4},
{ 17, 15, 50, 4},
{ 18, 16, 50, 4},
{ 19, 16, 50, 4},
{ 20, 18, 50, 4},
{ 30, 25, 55, 5},
{ 35, 70, 75, 5},

{ 26, 26, 50, 75 },
{ 28, 28, 70, 150 },
{ 32, 32, 60, 75 },
{ 36, 35, 60, 75 },
{ 38, 35, 70, 75 },
{ 40, 60, 80, 115 },
{ 45, 45, 80, 125 },
{ 48, 64, 80, 150 },

{ 10, 10, 50, 40 },
{ 25, 25, 50, 50 },
{ 40, 65, 80, 115 },
{ 44, 84, 80, 225 },
{ 46, 64, 80, 115 },
{ 48, 40, 80, 100 },
{ 49, 100, 90, 200 },
{ 50, 100, 80, 250 }
},
/* Mystic: Sorcery */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Mystic: Nature Magic */
{
{ 1, 1, 35, 2 },
{ 3, 3, 40, 2 },
{ 4, 4, 40, 3 },      
{ 7, 7, 55, 2 },
{ 7, 7, 50, 3 },
{ 7, 7, 50, 3 },
{ 8, 8, 50, 3 },
{ 9, 9, 45, 3 },

{ 10, 8, 80, 4 },
{ 11, 9, 40, 3 },
{ 12, 10, 40, 4 },
{ 14, 12, 55, 4 },
{ 16, 12, 55, 4 },
{ 18, 22, 65, 8 },
{ 31, 31, 75, 10 },
{ 40, 100, 95, 50 },

{ 12, 12, 50, 25 },
{ 14, 14, 50, 25 },
{ 16, 16, 70, 60 },
{ 22, 30, 85, 70 },
{ 35, 35, 80, 80 },
{ 40, 40, 90, 200 },
{ 45, 55, 90, 250 },
{ 50, 80, 90, 250 },

{ 28, 28, 60, 150 },
{ 30, 30, 60, 160 },
{ 35, 50, 75, 125 },
{ 33, 33, 75, 35 },
{ 38, 38, 75, 45 },
{ 42, 40, 75, 100 },
{ 45, 85, 95, 250 },
{ 48, 85, 75, 150 }
},
/* Mystic: Chaos Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Mystic: Death Magic */
{
{ 1, 1, 25, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 25, 4 },
{ 5, 5, 27, 4 },
{ 7, 7, 30, 4 },
{ 11, 11, 75, 6 },
{ 12, 12, 30, 4 },
{ 14, 14, 40, 5 },

{ 16, 16, 40, 5 },
{ 19, 19, 30, 4 },
{ 22, 22, 50, 10 },
{ 25, 25, 60, 16 },
{ 33, 80, 90, 30 },
{ 40, 40, 60, 16 },
{ 45, 45, 95, 100 },
{ 50, 60, 95, 150 },

{ 15, 20, 80, 180 },
{ 16, 16, 80, 30 },
{ 18, 18, 30, 15 },
{ 35, 35, 75, 50 },
{ 40, 40, 60, 125 },
{ 42, 95, 70, 90 },
{ 48, 50, 80, 200 },
{ 49, 80, 80, 125 },

{ 30, 30, 75, 50 },
{ 37, 85, 85, 220 },
{ 38, 50, 95, 250 },
{ 44, 44, 70, 40 },
{ 46, 50, 80, 70 },
{ 50, 140, 95, 250 },
{ 50, 100, 95, 250},
{ 50, 115, 95, 250 }
},
/* Mystic: No Planar magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

},
/* Mystic: No Folk Magic */
{
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

{ 99, 0, 0, 0},
{ 99, 0, 0, 0},
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                                
{ 99, 0, 0, 0},                        

},

/* Mystic: Corporeal */


{
{ 1, 1, 23, 4 },
{ 2, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 30, 1 },
{ 6, 6, 35, 5 },
{ 7, 7, 30, 4 },
{ 8, 8, 75, 9 },

{ 10, 9, 75, 8 },
{ 11, 10, 75, 8 },
{ 12, 11, 75, 7 },
{ 13, 12, 50, 6 },
{ 20, 15, 60, 8 },
{ 27, 18, 60, 8 },
{ 33, 25, 70, 15 },
{ 40, 40, 75, 20 },

{ 4, 4, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 12, 80, 40 },
{ 15, 12, 70, 30 },
{ 16, 14, 60, 25 },
{ 19, 19, 85, 50 },
{ 24, 22, 60, 25 },
{ 28, 28, 75, 19 },

{ 12, 12, 40, 20 },
{ 19, 19, 75, 70 },
{ 30, 35, 95, 160 },
{ 35, 45, 80, 120 },
{ 42, 85, 95, 200 },
{ 45, 100, 95, 200 },
{ 46, 55, 90, 175 },
{ 48, 75, 75, 250 },
},
}
},

{
/*** Mindcrafter ***/

TV_LIFE_BOOK,
0,

A_WIS,
0,

99,
300,

{
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},

{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},

{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},

{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}


},
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},
{

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}

},
},
},




{
/*** High Mage ***/

TV_SORCERY_BOOK,
0,

A_INT,
0,

1,
300,

/* High Mage: Life Magic */
{
{
{ 1, 1, 20, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 25, 4 },
{ 4, 4, 25, 4 },
{ 5, 5, 25, 4 },
{ 6, 5, 30, 4 },
{ 9, 9, 30, 3 },
{ 12, 10, 35, 3 },

{ 14, 12, 35, 4},
{ 15, 14, 40, 4},
{ 15, 15, 40, 4},
{ 17, 15, 40, 4},
{ 19, 17, 40, 4},
{ 21, 19, 40, 4},
{ 25, 25, 45, 5},
{ 30, 50, 55, 5},

{ 20, 20, 40, 75 },
{ 24, 24, 60, 150 },
{ 30, 30, 50, 75 },
{ 31, 30, 50, 75 },
{ 32, 30, 60, 75 },
{ 33, 40, 60, 115 },
{ 35, 35, 60, 125 },
{ 40, 70, 70, 150 },

{ 5, 5, 40, 40 },
{ 20, 20, 40, 50 },
{ 30, 70, 60, 115 },
{ 40, 80, 60, 225 },
{ 42, 75, 60, 115 },
{ 45, 40, 60, 100 },
{ 47, 90, 70, 250 },
{ 49, 90, 70, 250 }

},


/* High Mage: Sorcery */


{
{ 1, 1, 15, 4 },
{ 1, 1, 15, 4 },
{ 2, 2, 15, 1 },
{ 2, 2, 20, 1 },
{ 3, 3, 20, 1 },
{ 4, 3, 25, 5 },
{ 5, 4, 20, 4 },
{ 5, 5, 65, 9 },

{ 7, 5, 65, 8 },
{ 7, 5, 65, 8 },
{ 9, 5, 65, 7 },
{ 9, 5, 40, 6 },
{ 13, 8, 50, 8 },
{ 17, 10, 50, 8 },
{ 24, 15, 60, 15 },
{ 28, 20, 65, 20 },

{ 2, 2, 20, 15 },
{ 7, 7, 60, 40 },
{ 8, 8, 70, 40 },
{ 9, 9, 70, 40 },
{ 12, 9, 50, 25 },
{ 15, 12, 65, 50 },
{ 17, 12, 50, 25 },
{ 20, 20, 65, 19 },

{ 8, 8, 30, 20 },
{ 20, 20, 65, 70 },
{ 20, 25, 85, 160 },
{ 25, 30, 70, 120 },
{ 30, 65, 85, 200 },
{ 35, 80, 85, 200 },
{ 40, 40, 80, 175 },
{ 42, 65, 65, 250 },
},
/* High Mage: Nature Magic */
{
{ 1, 1, 15, 4 },
{ 2, 1, 15, 3 },
{ 2, 2, 15, 1 },
{ 3, 2, 25, 4 },
{ 3, 3, 40, 5 },
{ 4, 3, 40, 5 },
{ 4, 4, 40, 5 },
{ 5, 4, 25, 4 },

{ 5, 4, 30, 6 },
{ 5, 4, 20, 6 },
{ 5, 5, 35, 6 },
{ 5, 5, 30, 6 },
{ 7, 5, 30, 5 },
{ 14, 10, 35, 8 },
{ 20, 20, 80, 50 },
{ 35, 80, 85, 50 },

{ 5, 5, 10, 28 },
{ 7, 7, 20, 44 },
{ 8, 8, 65, 120 },
{ 12, 15, 75, 60 },
{ 25, 25, 80, 100 },
{ 33, 35, 80, 200 },
{ 35, 40, 65, 200},
{ 37, 65, 80, 250 },


{ 15, 15, 50, 25 },
{ 20, 20, 70, 50 },
{ 22, 22, 65, 29 },
{ 28, 25, 65, 35 },
{ 32, 28, 75, 65 },
{ 34, 30, 80, 100 },
{ 36, 80, 85, 250 },
{ 39, 65, 55, 150 }
},

/* High Mage: Chaos Magic */
{
{ 1, 1, 15, 4 },
{ 1, 1, 15, 4 },
{ 2, 1, 15, 4 },
{ 4, 2, 20, 1 },
{ 6, 4, 40, 1 },
{ 10, 5, 35, 6 },
{ 12, 6, 35, 6 },
{ 14, 7, 25, 5 },

{ 15, 9, 20, 5 },
{ 17, 10, 35, 9 },
{ 19, 11, 35, 10 },
{ 21, 12, 40, 11 },
{ 22, 13, 40, 12 },
{ 23, 15, 50, 8 },
{ 27, 17, 70, 15 },
{ 30, 35, 75, 40 },

{ 9, 5, 35, 9 },
{ 12, 12, 70, 35 },
{ 14, 12, 70, 35 },
{ 21, 21, 75, 100 },
{ 26, 22, 75, 150 },
{ 39, 40, 75, 250 },
{ 42, 80, 70, 250 },
{ 44, 90, 80, 250 },

{ 16, 16, 55, 8 },
{ 32, 30, 75, 35 },
{ 34, 32, 65, 40 },
{ 36, 36, 75, 100 },
{ 38, 38, 70, 150 },
{ 40, 45, 75, 200 },
{ 43, 55, 70, 200 },
{ 46, 90, 75, 250 }
},

/* High Mage: Death Magic */
{
{ 1, 1, 20, 4 },
{ 1, 1, 20, 4 },
{ 2, 1, 20, 4 },
{ 2, 2, 20, 3 },
{ 3, 3, 20, 4 },
{ 5, 9, 55, 6 },
{ 7, 7, 20, 4 },
{ 8, 8, 20, 4 },

{ 10, 10, 30, 5 },
{ 11, 10, 20, 4 },
{ 14, 12, 40, 10 },
{ 20, 16, 50, 16 },
{ 26, 65, 40, 30 },
{ 30, 30, 50, 16 },
{ 33, 30, 85, 25 },
{ 40, 40, 85, 150 },

{ 8, 15, 70, 180 },
{ 8, 10, 70, 30 },
{ 9, 9, 20, 15 },
{ 25, 20, 65, 50 },
{ 30, 30, 50, 125 },
{ 30, 80, 60, 90 },
{ 36, 35, 60, 200 },
{ 38, 66, 70, 100 },

{ 16, 16, 65, 50 },
{ 22, 60, 85 , 250 },
{ 26, 35, 85, 250 },
{ 29, 30, 60, 40 },
{ 33, 30, 70, 70 },
{ 39, 100, 85, 250 },
{ 41, 85, 80, 250 },
{ 44, 75, 80, 250 }
},

/* High Mage: Planar Magic */
{
{ 1, 1, 40, 3 },
{ 2, 2, 40, 4 },
{ 4, 4, 65, 8 },
{ 5, 5, 70, 8 },
{ 6, 5, 30, 4 },
{ 7, 7, 50, 6 },
{ 10, 10, 50, 6 },
{ 14, 12, 50, 5 },

{ 16, 16, 70, 8 },
{ 20, 20, 50, 8 },
{ 25, 22, 50, 8 },
{ 28, 24, 60, 9 },
{ 28, 26, 70, 12 },
{ 30, 28, 60, 10 },
{ 35, 30, 70, 15 },
{ 39, 36, 60, 12 },

{ 11, 11, 70, 20 },
{ 21, 21, 60, 25 },
{ 23, 23, 60, 30 },
{ 25, 25,  60, 35 },
{ 31, 65, 70, 100 },
{ 36, 90, 80, 250 },
{ 39, 45, 40, 75 },
{ 42, 90, 80, 200 },

{ 25, 25, 50, 50 },
{ 32, 45, 80, 100 },
{ 34, 75, 70, 150 },
{ 36, 75, 70, 150 },
{ 38, 90, 70, 200 },
{ 42, 90, 70, 150 },
{ 44, 90, 70, 200 },
{ 46, 90, 70, 220 }

},

/* High Mage: Folk Magic */
{
{ 1, 1, 15, 4 },
{ 1, 1, 23, 5 },
{ 1, 1, 23, 4 },
{ 1, 1, 23, 5 },
{ 2, 1, 23, 5 },
{ 3, 2, 33, 6 },
{ 4, 4, 23, 7 },
{ 5, 4, 33, 5 },

{ 6, 5, 30, 7 },
{ 7, 7, 50, 7 },
{ 8, 7, 40, 6 },
{ 8, 8, 40, 6 },
{ 9, 8, 40, 6 },
{ 10, 9, 40, 6 },
{ 10, 10, 40, 5 },
{ 11, 10, 40, 5 },

{ 12, 10, 40, 5 },
{ 13, 10, 40, 5 },
{ 14, 11, 22, 6 },
{ 15, 12, 40, 8 },
{ 17, 15, 50, 9 },
{ 20, 15, 50, 9 },
{ 20, 16, 60, 12 },
{ 22, 18, 50, 13 },

{ 24, 22, 60, 30 },
{ 30, 30, 70, 25 },
{ 33, 28, 50, 25 },
{ 38, 28, 60, 25 },
{ 40, 28, 55, 30 },
{ 41, 28, 70, 40 },
{ 43, 40, 60, 50 },
{ 46, 80, 70, 200 }

},
/* High Mage: Corporeal */


{
{ 1, 1, 15, 4 },
{ 1, 1, 15, 4 },
{ 2, 2, 15, 1 },
{ 2, 2, 20, 1 },
{ 3, 3, 20, 1 },
{ 4, 3, 25, 5 },
{ 5, 4, 20, 4 },
{ 5, 5, 65, 9 },

{ 7, 5, 65, 8 },
{ 7, 5, 65, 8 },
{ 9, 5, 65, 7 },
{ 9, 5, 40, 6 },
{ 13, 8, 50, 8 },
{ 17, 10, 50, 8 },
{ 24, 15, 60, 15 },
{ 28, 20, 65, 20 },

{ 2, 2, 20, 15 },
{ 7, 7, 60, 40 },
{ 8, 8, 70, 40 },
{ 9, 9, 70, 40 },
{ 12, 9, 50, 25 },
{ 15, 12, 65, 50 },
{ 17, 12, 50, 25 },
{ 20, 20, 65, 19 },

{ 8, 8, 30, 20 },
{ 20, 20, 65, 70 },
{ 20, 25, 85, 160 },
{ 25, 30, 70, 120 },
{ 30, 65, 85, 200 },
{ 35, 80, 85, 200 },
{ 40, 40, 80, 175 },
{ 42, 65, 65, 250 },
},

}
},
{
/*** Druid ***/
TV_LIFE_BOOK,
0,

A_WIS,
1,

1,
350,
{
/* Druid: No Life Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: No Sorcery */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: Nature Magic */
{
{ 1, 1, 15, 4 },
{ 2, 1, 15, 3 },
{ 2, 2, 15, 1 },
{ 3, 2, 25, 4 },
{ 3, 3, 40, 5 },
{ 4, 3, 40, 5 },
{ 4, 4, 40, 5 },
{ 5, 4, 25, 4 },

{ 5, 4, 30, 6 },
{ 5, 4, 20, 6 },
{ 5, 5, 35, 6 },
{ 5, 5, 30, 6 },
{ 7, 5, 30, 5 },
{ 14, 10, 35, 8 },
{ 20, 20, 80, 50 },
{ 35, 80, 85, 50 },

{ 5, 5, 10, 28 },
{ 7, 7, 20, 44 },
{ 8, 8, 65, 120 },
{ 12, 15, 75, 60 },
{ 25, 25, 80, 100 },
{ 33, 35, 80, 200 },
{ 35, 40, 65, 200},
{ 37, 65, 80, 250 },


{ 15, 15, 50, 25 },
{ 20, 20, 70, 50 },
{ 22, 22, 65, 29 },
{ 28, 25, 65, 35 },
{ 32, 28, 75, 65 },
{ 34, 30, 80, 100 },
{ 36, 80, 85, 250 },
{ 39, 65, 55, 150 }
},
/* Druid: No Chaos Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: No Death Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: No Planar Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: No Folk Magic */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
/* Druid: No Corporeal */
{
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},

{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0},
{ 99,  0,  0,   0}
},
}
},

{
/*** Demonologist ***/

TV_SORCERY_BOOK,
0,

A_INT,
0,

1,
300,

/* Demonologist: Life Magic */
{
{
{ 2, 2, 30, 4 },
{ 4, 4, 35, 4 },
{ 5, 5, 35, 4 },
{ 6, 6, 35, 4 },
{ 8, 8, 35, 4 },
{ 9, 9, 40, 4 },
{ 14, 14, 40, 3 },
{ 16, 16, 45, 3 },

{ 18, 18, 45, 4},
{ 20, 20, 50, 4},
{ 22, 22, 50, 4},
{ 24, 24, 50, 4},
{ 26, 26, 50, 4},
{ 28, 28, 50, 4},
{ 33, 33, 55, 5},
{ 40, 70, 75, 5},

{ 28, 28, 50, 75 },
{ 30, 30, 70, 150 },
{ 34, 34, 60, 75 },
{ 36, 36, 60, 75 },
{ 38, 38, 70, 75 },
{ 42, 55, 80, 115 },
{ 45, 45, 80, 125 },
{ 50, 70, 80, 150 },

{ 10, 10, 50, 40 },
{ 28, 28, 50, 50 },
{ 38, 85, 80, 115 },
{ 45, 90, 80, 225 },
{ 46, 90, 80, 115 },
{ 48, 50, 80, 100 },
{ 49, 100, 90, 100 },
{ 50, 100, 80, 250 }

},


/* Demonologist: Sorcery */


{
{ 1, 1, 23, 4 },
{ 2, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 30, 1 },
{ 6, 6, 35, 5 },
{ 7, 7, 30, 4 },
{ 8, 8, 75, 9 },

{ 10, 9, 75, 8 },
{ 11, 10, 75, 8 },
{ 12, 11, 75, 7 },
{ 13, 12, 50, 6 },
{ 20, 15, 60, 8 },
{ 27, 18, 60, 8 },
{ 33, 25, 70, 15 },
{ 40, 40, 75, 20 },

{ 4, 4, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 12, 80, 40 },
{ 15, 12, 70, 30 },
{ 16, 14, 60, 25 },
{ 19, 19, 85, 50 },
{ 24, 22, 60, 25 },
{ 28, 28, 75, 19 },

{ 12, 12, 40, 20 },
{ 19, 19, 75, 70 },
{ 30, 35, 95, 160 },
{ 35, 45, 80, 120 },
{ 42, 85, 95, 200 },
{ 45, 100, 95, 200 },
{ 46, 55, 90, 175 },
{ 48, 75, 75, 250 },
},

/* Demonologist: Nature Magic */

{
{ 2, 2, 23, 4 },
{ 3, 3, 25, 3 },
{ 4, 4, 25, 1 },
{ 5, 5, 35, 4 },
{ 6, 6, 50, 5 },
{ 7, 7, 50, 5 },
{ 8, 8, 50, 5 },
{ 9, 9, 35, 4 },

{ 10, 10, 40, 6 },
{ 11, 11, 30, 6 },
{ 12, 12, 45, 6 },
{ 13, 13, 40, 6 },
{ 14, 14, 30, 5 },
{ 19, 15, 65, 7 },
{ 31, 31, 65, 10 },
{ 45, 100, 95, 50 },

{ 9, 9, 20, 28 },
{ 12, 12, 40, 44 },
{ 15, 15, 75, 120 },
{ 20, 22, 85, 60 },
{ 38, 38, 85, 80 },
{ 40, 42, 90, 200 },
{ 45, 48, 75, 200},
{ 49, 95, 90, 250 },


{ 25, 25, 60, 25 },
{ 27, 27, 60, 25 },
{ 28, 28, 75, 29 },
{ 33, 33, 75, 35 },
{ 38, 38, 85, 65 },
{ 41, 41, 90, 100 },
{ 45, 95, 95, 250 },
{ 50, 85, 65, 150 },
},

/* Demonologist: Chaos Magic */
{
{ 1, 1, 15, 4 },
{ 1, 1, 15, 4 },
{ 2, 1, 15, 4 },
{ 4, 2, 20, 1 },
{ 6, 4, 40, 1 },
{ 10, 5, 35, 6 },
{ 12, 6, 35, 6 },
{ 14, 7, 25, 5 },

{ 15, 9, 20, 5 },
{ 17, 10, 35, 9 },
{ 19, 11, 35, 10 },
{ 21, 12, 40, 11 },
{ 22, 13, 40, 12 },
{ 23, 15, 50, 8 },
{ 27, 17, 70, 15 },
{ 30, 35, 75, 40 },

{ 9, 5, 35, 9 },
{ 12, 12, 70, 35 },
{ 14, 12, 70, 35 },
{ 21, 21, 75, 100 },
{ 26, 22, 75, 150 },
{ 39, 40, 75, 250 },
{ 42, 80, 70, 250 },
{ 44, 90, 80, 250 },

{ 16, 16, 55, 8 },
{ 32, 30, 75, 35 },
{ 34, 32, 65, 40 },
{ 36, 36, 75, 100 },
{ 38, 38, 70, 150 },
{ 40, 45, 75, 200 },
{ 43, 55, 70, 200 },
{ 46, 90, 75, 250 }
},

/* Demonologist: Death Magic */
{
{ 1, 1, 25, 4 },
{ 2, 2, 25, 4 },
{ 3, 3, 25, 4 },
{ 4, 4, 27, 3 },
{ 7, 7, 30, 4 },
{ 9, 10, 75, 6 },
{ 10, 10, 30, 4 },
{ 12, 12, 30, 4 },

{ 14, 14, 40, 5 },
{ 16, 16, 30, 4 },
{ 21, 21, 50, 10 },
{ 25, 25, 60, 16 },
{ 35, 75, 50, 30 },
{ 40, 40, 60, 16 },
{ 44, 45, 95, 25 },
{ 48, 55, 95, 150 },

{ 10, 22, 80, 180 },
{ 12, 18, 80, 30 },
{ 14, 18, 30, 15 },
{ 30, 30, 75, 50 },
{ 40, 40, 60, 125 },
{ 42, 90, 70, 90 },
{ 45, 50, 70, 200 },
{ 48, 85, 80, 100 },

{ 24, 24, 75, 50 },
{ 33, 80, 75, 150 },
{ 35, 45, 95, 250 },
{ 42, 50, 70, 40 },
{ 45, 55, 80, 70 },
{ 50, 135, 95, 250 },
{ 50, 100, 95, 250 },
{ 50, 123, 95, 250 },
},

/* Demonologist: Planar Magic */
{
{ 1, 1, 50, 3 },
{ 5, 5, 50, 4 },
{ 7, 7, 75, 8 },
{ 8, 7, 80, 8 },
{ 10, 10, 40, 4 },
{ 12, 12, 60, 6 },
{ 18, 15, 60, 6 },
{ 20, 18, 60, 5 },

{ 24, 23, 80, 8 },
{ 28, 25, 60, 8 },
{ 31, 26, 60, 8 },
{ 33, 30, 70, 9 },
{ 38, 32, 80, 12 },
{ 40, 38, 70, 10 },
{ 44, 42, 80, 15 },
{ 48, 46, 70, 12 },

{ 19, 18, 80, 20 },
{ 29, 27, 70, 25 },
{ 31, 30, 70, 30 },
{ 35, 33,  70, 35 },
{ 40, 80, 80, 100 },
{ 42, 120, 90, 250 },
{ 46, 55, 50, 75 },
{ 50, 135, 90, 200 },

{ 33, 30, 60, 50 },
{ 40, 60, 90, 100 },
{ 42, 95, 80, 150 },
{ 45, 95, 80, 150 },
{ 46, 120, 80, 200 },
{ 48, 125, 80, 150 },
{ 49, 130, 80, 200 },
{ 50, 135, 80, 220 }

},

/* Demonologist: Folk Magic */
{
{ 1, 1, 20, 4 },
{ 2, 1, 33, 5 },
{ 2, 2, 33, 4 },
{ 3, 3, 33, 5 },
{ 4, 4, 33, 5 },
{ 5, 5, 40, 6 },
{ 6, 6, 33, 7 },
{ 7, 7, 44, 5 },

{ 8, 8, 40, 7 },
{ 9, 9, 60, 7 },
{ 11, 10, 50, 6 },
{ 12, 11, 50, 6 },
{ 13, 12, 50, 6 },
{ 14, 13, 50, 6 },
{ 15, 14, 50, 5 },
{ 16, 15, 50, 5 },

{ 17, 16, 50, 5 },
{ 18, 17, 50, 5 },
{ 19, 18, 33, 6 },
{ 20, 20, 50, 8 },
{ 23, 22, 60, 9 },
{ 25, 23, 60, 9 },
{ 29, 25, 70, 12 },
{ 30, 27, 60, 13 },

{ 35, 30, 80, 50 },
{ 39, 38, 80, 25 },
{ 41, 40, 60, 25 },
{ 43, 42, 70, 25 },
{ 45, 44, 66, 30 },
{ 47, 45, 80, 40 },
{ 48, 65, 70, 50 },
{ 50, 140, 80, 200 }

},
/* Demonologist: Corporeal */


{
{ 1, 1, 23, 4 },
{ 2, 2, 24, 4 },
{ 3, 3, 25, 1 },
{ 4, 4, 30, 1 },
{ 5, 5, 30, 1 },
{ 6, 6, 35, 5 },
{ 7, 7, 30, 4 },
{ 8, 8, 75, 9 },

{ 10, 9, 75, 8 },
{ 11, 10, 75, 8 },
{ 12, 11, 75, 7 },
{ 13, 12, 50, 6 },
{ 20, 15, 60, 8 },
{ 27, 18, 60, 8 },
{ 33, 25, 70, 15 },
{ 40, 40, 75, 20 },

{ 4, 4, 25, 15 },
{ 12, 12, 70, 40 },
{ 14, 12, 80, 40 },
{ 15, 12, 70, 30 },
{ 16, 14, 60, 25 },
{ 19, 19, 85, 50 },
{ 24, 22, 60, 25 },
{ 28, 28, 75, 19 },

{ 12, 12, 40, 20 },
{ 19, 19, 75, 70 },
{ 30, 35, 95, 160 },
{ 35, 45, 80, 120 },
{ 42, 85, 95, 200 },
{ 45, 100, 95, 200 },
{ 46, 55, 90, 175 },
{ 48, 75, 75, 250 },
},

}
},




};

u32b fake_spell_flags[4]=
{
0x000000ff,
0x0000ff00,
0x00ff0000,
0xff000000
};

byte realm_choices[MAX_CLASS]=
{
/* Warrior */		(CH_NONE),
/* Mage */			(CH_LIFE | CH_SORCERY | CH_NATURE | CH_CHAOS | CH_DEATH | CH_PLANAR | CH_FOLK | CH_CORPOREAL),
/* Priest */		(CH_NATURE | CH_CHAOS | CH_PLANAR | CH_FOLK | CH_CORPOREAL),
/* Rogue */			(CH_SORCERY | CH_DEATH | CH_PLANAR | CH_FOLK ),
/* Ranger */		(CH_CHAOS | CH_DEATH | CH_PLANAR | CH_FOLK | CH_CORPOREAL),
/* Paladin */		(CH_LIFE | CH_DEATH),
/* Warrior-Mage */  (CH_LIFE | CH_NATURE | CH_CHAOS | CH_DEATH | CH_PLANAR | CH_FOLK | CH_SORCERY | CH_CORPOREAL),
/* Diabolist */		(CH_CHAOS),
/* Mystic */        (CH_CORPOREAL),
/* Mindcrafter */   (CH_NONE),
/* High Mage */     (CH_LIFE | CH_SORCERY | CH_NATURE | CH_CHAOS | CH_DEATH | CH_PLANAR | CH_FOLK | CH_CORPOREAL),
/* Druid */         (CH_NATURE),
/* Demonologist */  (CH_LIFE | CH_SORCERY | CH_NATURE | CH_DEATH | CH_PLANAR | CH_FOLK | CH_CORPOREAL),
};

cptr realm_names [] =
{
"no magic",
"Life",
"Sorcery",
"Nature",
"Chaos",
"Death",
"Planar",
"Folk",
"Corporeal"
};


/*
* Names of the spells (mage spells then priest spells)
*/
cptr spell_names[MAX_REALM][32] =
{
/*** Life Spells ***/

{
/* Common Life Spellbooks */
"Detect Evil",
"Cure Light Wounds",
"Bless",
"Remove Fear",
"Call Light",
"Detect Traps and Secret Doors",
"Cure Medium Wounds",
"Satisfy Hunger",

"Remove Curse",
"Cure Poison",   
"Cure Critical Wounds",
"Sense Unseen",
"Holy Orb",
"Protection from Evil",
"Healing",
"Glyph of Warding",

/* Rare Life Spellbooks */
"Exorcism",
"Dispel Curse",
"Dispel Undead & Demons",
"Day of the Dove",
"Dispel Evil",
"Banish",      
"Holy Word",
"Warding True",

"Heroism",
"Prayer",
"Bless Weapon",
"Restoration",
"Healing True",
"Holy Vision",
"Divine Intervention",
"Holy Invulnerability"

},

/*** Sorcery Spells ***/

{
/* Common Sorcery Spellbooks */
"Detect Monsters",
"Phase Door",
"Detect Doors and Traps",
"Light Area",
"Confuse Monster",
"Teleport",
"Sleep Monster",
"Recharging",

"Magic Mapping",
"Identify",
"Slow Monster",
"Mass Sleep",
"Teleport Away",
"Haste Self",
"Detection True",
"Identify True",

/* Rare Sorcery Spellbooks */
"Detect Objects and Treasure",
"Detect Enchantment",
"Charm Monster",
"Dimension Door",
"Sense Minds",
"Self Knowledge",
"Teleport Level",
"Word of Recall",

"Stasis",
"Telekinesis",
"Explosive Rune",
"Clairvoyance",
"Enchant Weapon",
"Enchant Armour",
"Alchemy",
"Globe of Invulnerability"

},

/*** Nature Spellbooks ***/

{
/* Common Nature Spellbooks */
"Detect Creatures",
"First Aid",
"Detect Doors and Traps",
"Foraging",
"Daylight",
"Animal Taming",
"Resist Environment",
"Cure Wounds & Poison",

"Stone to Mud",
"Lightning Bolt",
"Nature Awareness",
"Frost Bolt",
"Ray of Sunlight",
"Entangle",
"Summon Animal",
"Herbal Healing",

/* Rare Nature Spellbooks */
"Door Building",
"Stair Building",
"Stone Skin",
"Resistance True",
"Animal Friendship",
"Stone Tell",
"Wall of Stone",
"Protect from Corrosion",

"Earthquake",
"Whirlwind Attack",
"Blizzard",
"Lightning Storm",
"Whirlpool",
"Call Sunlight",
"Elemental Branding",
"Nature's Wrath"

},

/*** Chaos Spells ***/

{
/* Common Chaos Spellbooks */
"Magic Missile",
"Trap / Door Destruction",
"Flash of Light",
"Touch of Confusion",
"Mana Burst",
"Fire Bolt",
"Fist of Force",
"Teleport Self",

"Wonder",
"Chaos Bolt",
"Sonic Boom",
"Doom Bolt",
"Fire Ball",
"Teleport Other",
"Word of Destruction",
"Invoke Chaos",

/* Rare Chaos Spellbooks */
"Polymorph Other",
"Chain Lightning",
"Arcane Binding",
"Disintegrate",
"Alter Reality",
"Polymorph Self",
"Chaos Branding",
"Summon Demon",

"Beam of Gravity",
"Meteor Swarm",
"Flame Strike",
"Call Chaos",
"Shard Ball",
"Mana Storm",
"Breathe Chaos",
"Call the Void"
},

/*** Death Spells ***/

{
/* Common Death Spellbooks */
"Detect Unlife",
"Malediction",
"Detect Evil",
"Stinking Cloud",
"Black Sleep",
"Resist Poison",
"Horrify",
"Enslave Undead",

"Orb of Entropy",
"Nether Bolt",
"Terror",
"Vampiric Drain",
"Poison Branding",
"Dispel Good",
"Genocide",
"Restore Life",

/* Rare Death Spellbooks */
"Berserk",
"Invoke Spirits",
"Dark Bolt",
"Battle Frenzy",
"Vampirism True",
"Vampiric Branding",
"Darkness Storm",
"Mass Genocide",

"Death Ray",
"Raise the Dead",
"Esoteria",
"Word of Death",
"Evocation",
"Hellfire",
"Omnicide",
"Wraithform"
},

/* Planar Spellbooks */
{
/* Common Planar Spellbooks */
"Phase Door",
"Mind Blast",
"Tarot Draw",
"Reset Recall",
"Teleport",
"Dimension Door",
"Planar Spying",
"Teleport Away",

"Summon Object",
"Summon Animal",
"Phantasmal Servant",
"Summon Monster",
"Conjure Elemental",
"Teleport Level",
"Word of Recall",
"Banish",

/* Rare Planar Spellbooks */
"Joker Card",
"Summon Spiders",
"Summon Reptiles",
"Summon Hounds",
"Planar Branding",
"Planar Being",
"Death Dealing",
"Summon Reaver",

"Planar Divination",
"Planar Lore",
"Summon Undead",
"Summon Dragon",
"Mass Summons",
"Summon Demon",
"Summon Ancient Dragon",
"Summon Greater Undead"
},

/* Folk Spellbooks (_only_ common spells) */
{
"Zap",
"Wizard Lock",
"Detect Invisibility",
"Detect Monsters",
"Blink",
"Light Area",
"Trap & Door Destruction",
"Cure Light Wounds",

"Detect Doors & Traps",
"Phlogiston",
"Detect Treasure",
"Detect Enchantment",
"Detect Objects",
"Cure Poison",
"Resist Cold",
"Resist Fire",

"Resist Lightning",
"Resist Acid",
"Cure Medium Wounds",
"Teleport",
"Stone to Mud",
"Ray of Light",
"Satisfy Hunger",
"See Invisible",

"Recharging",
"Teleport Level",
"Identify",
"Teleport Away",
"Elemental Ball",
"Detection",
"Word of Recall",
"Clairvoyance"
},
/* Corporeal Spellbooks */
{
/* Common Corporeal Spellbooks */
"Cure Light Wounds",
"Blink",
"Bravery",
"Bat's Sense",
"Eagle's Vision",
"Mind Vision",
"Cure Medium Wounds",
"Satisfy Hunger",

"Burn Resistance",
"Detoxify",
"Cure Critical Wounds",
"See Invisible",
"Teleport",
"Haste",
"Healing",
"Resist True",

/* Rare Corporeal Spellbooks */
"Horrific Visage",
"See Magic",
"Stone Skin",
"Move Body",
"Corrupt Body",
"Know Self",
"Teleport Level",
"Word of Recall",

"Heroism",
"Wraithform",
"Attune to Magic",
"Restore Body",
"Healing True",
"Hypnotic Eyes",
"Restore Soul",
"Invulnerability"
}
};



/*
* Each chest has a certain set of traps, determined by pval
* Each chest has a "pval" from 1 to the chest level (max 55)
* If the "pval" is negative then the trap has been disarmed
* The "pval" of a chest determines the quality of its treasure
* Note that disarming a trap on a chest also removes the lock.
*/
byte chest_traps[64] =
{
0,                                      /* 0 == empty */
(CHEST_POISON),
(CHEST_LOSE_STR),
(CHEST_LOSE_CON),
(CHEST_LOSE_STR),
(CHEST_LOSE_CON),                       /* 5 == best small wooden */
0,
(CHEST_POISON),
(CHEST_POISON),
(CHEST_LOSE_STR),
(CHEST_LOSE_CON),
(CHEST_POISON),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_SUMMON),                 /* 15 == best large wooden */
0,
(CHEST_LOSE_STR),
(CHEST_LOSE_CON),
(CHEST_PARALYZE),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_SUMMON),
(CHEST_PARALYZE),
(CHEST_LOSE_STR),
(CHEST_LOSE_CON),
(CHEST_EXPLODE),                        /* 25 == best small iron */
0,
(CHEST_POISON | CHEST_LOSE_STR),
(CHEST_POISON | CHEST_LOSE_CON),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_PARALYZE),
(CHEST_POISON | CHEST_SUMMON),
(CHEST_SUMMON),
(CHEST_EXPLODE),
(CHEST_EXPLODE | CHEST_SUMMON), /* 35 == best large iron */
0,
(CHEST_SUMMON),
(CHEST_EXPLODE),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_POISON | CHEST_PARALYZE),
(CHEST_EXPLODE),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_POISON | CHEST_PARALYZE),        /* 45 == best small steel */
0,
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR),
(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_CON),
(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_POISON | CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_POISON | CHEST_PARALYZE | CHEST_LOSE_STR | CHEST_LOSE_CON),
(CHEST_POISON | CHEST_PARALYZE),
(CHEST_POISON | CHEST_PARALYZE),        /* 55 == best large steel */
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
(CHEST_EXPLODE | CHEST_SUMMON),
};








/*
* Class titles for the player.
*
* The player gets a new title every five levels, so each class
* needs only ten titles total.
*/
cptr player_title[MAX_CLASS][PY_MAX_LEVEL/5] =
{
/* Warrior */
{
"Rookie",
"Soldier",
"Mercenary",
"Veteran",
"Swordsman",
"Champion",
"Hero",
"Baron",
"Duke",
"Lord",
},

/* Mage */
{
"Apprentice",
"Trickster",
"Illusionist",
"Spellbinder",
"Evoker",
"Conjurer",
"Warlock",
"Sorcerer",
"Ipsissimus",
"Archimage",
},

/* Priest */
{
"Believer",
"Acolyte",
"Adept",
"Curate",
"Canon",
"Priest",
"High Priest",
"Cardinal",
"Inquisitor",
"Pope",
},

/* Rogues */
{
"Cutpurse",
"Robber",
"Burglar",
"Filcher",
"Sharper",
"Low Thief",
"High Thief",
"Master Thief",
"Assassin",
"Guildmaster",
},

/* Rangers */
{
"Runner",
"Strider",
"Scout",
"Courser",
"Tracker",
"Guide",
"Pathfinder",
"Low Ranger",
"High Ranger",
"Ranger Lord",
},

/* Paladins */
{
"Gallant",
"Keeper",
"Protector",
"Defender",
"Warder",
"Knight",
"Guardian",
"Low Paladin",
"High Paladin",
"Paladin Lord",
},

/* Warrior-Mage */
{
"Novice",
"Apprentice",
"Journeyman",
"Veteran",
"Enchanter",
"Champion",
"Mage-Hero",
"Baron Mage",
"Battlemage",
"Wizard Lord",
},

/* Diabolist */
{
"Rookie",
"Soldier",
"Mercenary",
"Veteran",
"Swordsman",
"Champion",
"Chaos Hero",
"Chaos Baron",
"Chaos Duke",
"Chaos Lord",
},

/* Mystic */
{
"Initiate",
"Brother",
"Disciple",
"Immaculate",
"Master",
"Soft Master",
"Hard Master",
"Flower Master",
"Dragon Master",
"Grand Master",
},

/* Mindcrafter */
{
"Trainee",
"Acolyte",
"Adept",
"Immaculate",
"Contemplator",
"Mentalist",
"Psychic",
"Psionicist",
"Esper",
"Mindmaster",
},

/* High Mage; same as Mage */
{
"Apprentice",
"Trickster",
"Illusionist",
"Spellbinder",
"Evoker",
"Conjurer",
"Warlock",
"Sorcerer",
"Ipsissimus",
"Archimage",
},

/* Druid */
{
"Neophyte",
"Initiate",
"Adept",
"Lesser Druid",
"Druid",
"Silver Druid",
"Golden Druid",
"Great Druid",
"Arch-Druid",
"Grand Druid",
},
/* Demonologist */
{
"Apprentice",
"Trickster",
"Illusionist",
"Spellbinder",
"Entropist",
"Chaotic",
"Randomiser",
"Demonologist",
"Chaos-Archmage",
"Chaos Master",
},

};



/*
* Hack -- the "basic" colour names (see "TERM_xxx")
*/
cptr colour_names[16] =
{
"Dark",
"White",
"Slate",
"Orange",
"Red",
"Green",
"Blue",
"Umber",
"Light Dark",
"Light Slate",
"Violet",
"Yellow",
"Light Red",
"Light Green",
"Light Blue",
"Light Umber",
};


/*
* Abbreviations of healthy stats
*/
cptr stat_names[6] =
{
"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHA: "
};

/*
* Abbreviations of damaged stats
*/
cptr stat_names_reduced[6] =
{
"str: ", "int: ", "wis: ", "dex: ", "con: ", "cha: "
};


/*
* Certain "screens" always use the main screen, including News, Birth,
* Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
*
* Later, special flags may allow sub-windows to "steal" stuff from the
* main window, including File dump (help), File dump (artefacts, uniques),
* Character screen, Small scale map, Previous Messages, Store screen, etc.
*
* The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
* equip/inven" flags for all windows.
*
* The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
* of the main screen into any interested windows.
*/
cptr window_flag_desc[32] =
{
"Display inven/equip",
"Display equip/inven",
"Display spell list",
"Display character",
NULL,
NULL,
"Display messages",
"Display overhead view",
"Display monster recall",
"Display object recall",
NULL,
"Display snap-shot",
NULL,
NULL,
"Display borg messages",
"Display borg status",
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL,
NULL
};


/*
* Available Options
*/
option_type option_info[] =
{
/*** User-Interface ***/

{ &rogue_like_commands, FALSE,  1,      0, 0,
"rogue_like_commands",  "Rogue-like commands" },

{ &quick_messages,              TRUE,  1,      0, 1,
"quick_messages",               "Activate quick messages" },

{ &other_query_flag,    FALSE,  1,      0, 2,
"other_query_flag",             "Prompt for item confirmation" },

{ &carry_query_flag,    FALSE,  4,      0, 3,
"carry_query_flag",             "Prompt before picking things up" },

{ &use_old_target,              FALSE,  3,      0, 4,
"use_old_target",               "Use old target by default" },

{ &always_pickup,               TRUE,   4,      0, 5,
"always_pickup",                "Pick things up by default" },

{ &always_repeat,               TRUE,   1,      0, 6,
"always_repeat",                "Repeat obvious commands" },

{ &depth_in_feet,               FALSE,  1,      0, 7,
"depth_in_feet",                "Show dungeon level in feet" },

{ &stack_force_notes,   TRUE,  4,      0, 8,
"stack_force_notes",    "Merge inscriptions when stacking" },

{ &stack_force_costs,   TRUE,  4,      0, 9,
"stack_force_costs",    "Merge discounts when stacking" },

{ &show_labels,                 TRUE,   4,      0, 10,
"show_labels",                  "Show labels in object listings" },

{ &show_weights,                TRUE,  4,      0, 11,
"show_weights",                 "Show weights in object listings" },

{ &show_choices,                TRUE,  1,      0, 12,
"show_choices",                 "Show choices in certain sub-windows" },

{ &show_details,                TRUE,  1,      0, 13,
"show_details",                 "Show description in monster details" },

{ &ring_bell,                   FALSE,   1,      0, 14,
"ring_bell",                    "Audible bell (on errors, etc)" },
/* Changed to default to FALSE -- it's so extremely annoying!!! -TY */

{ &use_colour,                   TRUE,   1,      0, 15,
"use_colour",                    "Use colour if possible" },


/*** Disturbance ***/

{ &find_ignore_stairs,  FALSE,   2,      0, 16,
"find_ignore_stairs",   "Run past stairs" },

{ &find_ignore_doors,   TRUE,   2,      0, 17,
"find_ignore_doors",    "Run through open doors" },

{ &find_cut,                    TRUE,   2,      0, 18,
"find_cut",                             "Run past known corners" },

{ &find_examine,                TRUE,   2,      0, 19,
"find_examine",                 "Run into potential corners" },

{ &disturb_move,                FALSE,   2,      0, 20,
"disturb_move",                 "Disturb whenever any monster moves" },

{ &disturb_near,                TRUE,   2,      0, 21,
"disturb_near",                 "Disturb whenever viewable monster moves" },

{ &disturb_panel,               TRUE,   2,      0, 22,
"disturb_panel",                "Disturb whenever map panel changes" },

{ &disturb_state,               TRUE,   2,      0, 23,
"disturb_state",                "Disturb whenever player state changes" },

{ &disturb_minor,               FALSE,   2,      0, 24,
"disturb_minor",                "Disturb whenever boring things happen" },

{ &disturb_other,               TRUE,   2,      0, 25,
"disturb_other",                "Disturb whenever random things happen" },

{ &alert_hitpoint,              TRUE,  2,      0, 26,
"alert_hitpoint",               "Alert user to critical hitpoints" },

{ &alert_failure,               FALSE,  2,      0, 27,
"alert_failure",                "Alert user to various failures" },

{ &small_levels,                TRUE,   6,      0, 28,
"small_levels",              "Allow unusually small dungeon levels" },

{ &empty_levels,                TRUE,   6,      0, 29,
"empty_levels",             "Allow empty 'arena' levels" },

/*** Game-Play ***/

{ &auto_haggle,                 TRUE,  6,      1, 0,
"auto_haggle",                  "Auto-haggle in stores" },

{ &auto_scum,                   TRUE,  6,      1, 1,
"auto_scum",                    "Auto-scum for good levels" },

{ &stack_allow_items,   TRUE,   4,      1, 2,
"stack_allow_items",    "Allow weapons and armour to stack" },

{ &stack_allow_wands,   TRUE,   4,      1, 3,
"stack_allow_wands",    "Allow wands/staffs/rods to stack" },

{ &expand_look,                 TRUE,  6,      1, 4,
"expand_look",                  "Expand the power of the look command" },

{ &expand_list,                 TRUE,  6,      1, 5,
"expand_list",                  "Expand the power of the list commands" },

{ &view_perma_grids,    TRUE,   6,      1, 6,
"view_perma_grids",             "Map remembers all perma-lit grids" },

{ &view_torch_grids,    TRUE,  6,      1, 7,
"view_torch_grids",             "Map remembers all torch-lit grids" },

{ &dungeon_align,               TRUE,   6,      1, 8,
"dungeon_align",                "Generate dungeons with aligned rooms" },

{ &dungeon_stair,               TRUE,   6,      1, 9,
"dungeon_stair",                "Generate dungeons with connected stairs" },

{ &dungeon_small,               FALSE,   6,      1, 10,
"dungeon_small",                "Always generate small dungeons" },

{ &flow_by_sound,               FALSE,  3,      1, 11,
"flow_by_sound",                "Monsters chase current location" },

{ &flow_by_smell,               TRUE,  3,      1, 12,
"flow_by_smell",                "Monsters chase recent locations" },



{ &player_symbols,              TRUE,   1,      1, 13,
"player_symbols",             "Use special symbols for the player char"},

{ &equippy_chars,               TRUE,  4,      1, 14,
"equippy_chars",           "Display 'equippy' chars" },

{ &smart_learn,                 TRUE,  3,      1, 15,
"smart_learn",                  "Monsters learn from their mistakes" },

{ &smart_cheat,                 FALSE,  3,      1, 16,
"smart_cheat",                  "Monsters exploit players weaknesses" },

#ifdef ALLOW_EASY_OPEN /* TNB */
{ &easy_open,                   TRUE,  6,      1, 17,
"easy_open",                    "Open and close automatically" },
#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM /* TNB */
{ &easy_disarm,                 TRUE,  6,      1, 18,
"easy_disarm",                  "Disarm traps automatically" },
#endif /* ALLOW_EASY_DISARM -- TNB */


/*** Efficiency ***/

{ &view_reduce_lite,    FALSE,  5,      1, 19,
"view_reduce_lite",             "Reduce lite-radius when running" },

{ &view_reduce_view,    FALSE,  5,      1, 20,
"view_reduce_view",             "Reduce view-radius in town" },

{ &avoid_abort,                 FALSE,  5,      1, 21,
"avoid_abort",                  "Avoid checking for user abort" },

{ &avoid_other,                 FALSE,  5,      1, 22,
"avoid_other",                  "Avoid processing special colours" },

{ &flush_failure,               TRUE,   5,      1, 23,
"flush_failure",                "Flush input on various failures" },

{ &flush_disturb,               FALSE,  5,      1, 24,
"flush_disturb",                "Flush input whenever disturbed" },

{ &flush_command,               FALSE,  5,      1, 25,
"flush_command",                "Flush input before every command" },

{ &fresh_before,                TRUE,   5,      1, 26,
"fresh_before",                 "Refresh screen before every command" },

{ &fresh_after,                 FALSE,  5,      1, 27,
"fresh_after",                  "Refresh screen after every command" },

{ &fresh_message,               FALSE,  5,      1, 28,
"fresh_message",                "Refresh screen after every message" },

{ &compress_savefile,   TRUE,   5,      1, 29,
"compress_savefile",    "Compress messages in savefiles" },

{ &hilite_player,               FALSE,  1,      1, 30,
"hilite_player",                "Hilite the player with the cursor" },

{ &view_yellow_lite,    TRUE,  5,      1, 31,
"view_yellow_lite",             "Use special colours for torch-lit grids" },

{ &view_bright_lite,    TRUE,  5,      2, 1,
"view_bright_lite",             "Use special colours for 'viewable' grids" },

{ &view_granite_lite,   TRUE,  5,      2, 2,
"view_granite_lite",    "Use special colours for wall grids" },

{ &view_special_lite,   TRUE,  5,      2, 3,
"view_special_lite",    "Use special colours for floor grids" },

{ &skip_corruptions,        FALSE, 6, 2, 4,
"skip_corruptions",      "Skip corruptions in 'C'haracter Display" },

{ &plain_descriptions,    FALSE, 4, 2, 5,
"plain_descriptions", "Plain object descriptions" },

{ &stupid_monsters,      FALSE, 3, 2, 6,
"stupid_monsters",  "Monsters behave stupidly" },

{ &auto_destroy,        TRUE, 4, 2, 7,
"auto_destroy",     "No query to destroy known worthless items" },

{ &wear_confirm,        TRUE, 4,2, 8,
"confirm_wear",     "Confirm to wear/wield known cursed items" },

{ &confirm_stairs,      FALSE, 1, 2, 9,
"confirm_stairs",   "Prompt before exiting a dungeon level" },

{ &disturb_allies,        FALSE, 2, 2, 10,
"disturb_allies",     "Disturb when visible allies move" },

{ &multi_stair,        FALSE, 6, 2, 11,
"multi_stair",     "Stairs can be longer than one level" },

{ &rand_unbiased,    TRUE,5,2,12,
"rand_unbiased","Random numbers have bias removed"},

{ &unify_commands,  FALSE,4,2,13,
"unify_commands","Use a single 'u'se command for all objects"},

{ &testing_stack,               TRUE,  4, 2, 14,
"testing_stack",                "Allow objects to stack on floor" },

{ &monsters_carry,               TRUE,  3, 2, 15,
"monsters_carry",                "Allow monsters to carry objects" },

{ &centre_view,               TRUE,  1, 2, 16,
"centre_view",                "Centre view around player" },

{ &no_centre_run,               FALSE,  5, 2, 17,
"no_centre_run",                "Do not centre view whilst running" },

{ &maximise_mode,               TRUE,  7, 2, 18,
"maximise_mode",                "Include race/class bonuses in stat calcs" },

{ &preserve_mode,               TRUE,  7, 2, 19,
"preserve_mode",                "Artifacts are not lost if you never saw them" },

{ &use_autoroller,               FALSE,  7, 2, 20,
"use_autoroller",                "Stats are rolled repeatedly with minima" },

{ &spend_points,               TRUE,  7, 2, 21,
"spend_points",                "Stats are not rolled, points are spent on them" },

{ &ironman_shop,               FALSE,  7, 2, 22,
"ironman_shop",                "Shops (except for libraries) are locked" },


/*** End of Table ***/

{ NULL,                 0, 0, 0, 0,
NULL,                   NULL }
};


cptr evil_patron_shorts[MAX_PATRON] =
{
"Abaddon",
"Asmodeus",
"Astoreth",
"Baal",
"Balaam",

"Belial",
"Belphegor",
"Bifrons",
"Dispater",
"Hauras",

"Kobal",
"Mephistopheles",
"Nergal",
"Pazzuzzu",
"Sargatanas",

"Lucifer"
};

cptr evil_patron_longs[MAX_PATRON] =
{
"Abaddon, Leader of Hell's Army",
"Asmodeus, Prince of Lust",
"Astoreth, Treasurer of Hell",
"Baal, Duke of Sloth",
"Balaam the Prophet",

"Belial, Lord of Darkness",
"Belphegor the Temptress",
"Bifrons, Prince of Sorcery",
"Dispater of the Underworld",
"Hauras, Lord of Fire",

"Kobal, the Dark Entertainer",
"Mephistopheles the Dealmaker",
"Nergal the Inquisitor",
"Pazzuzzu, Lord of the Air",
"Sargatanas, Brigadeer of Infernal Spirits",

"Lucifer Morningstar"
};

int evil_stats[MAX_PATRON] =
{
A_CON,  /* Abaddon */
A_CON,  /* Asmodeus */
A_STR,  /* Astoreth */
A_STR,  /* Baal */
A_STR,  /* Balaam */

A_INT,  /* Belial */
A_STR,  /* Belphegor */
A_INT,  /* Bifrons */
A_CON,  /* Dispater */
A_CHA,  /* Hauras */

-1,     /* Kobal */
A_STR,  /* Mephistopheles */
A_CHA,  /* Nergal */
A_CON,  /* Pazzuzzu */
A_INT,  /* Sargatanas */

A_STR,  /* Lucifer */
};




int chaos_rewards[MAX_PATRON][20] =
{

/* Thed, Mother of Devilspawn: */
{
REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL, REW_LOSE_ABL,
REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_SLF,
REW_POLY_SLF, REW_POLY_SLF, REW_GAIN_ABL, REW_GAIN_ABL, REW_GAIN_EXP,
REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_AUGM_ABL, REW_AUGM_ABL
},

/* Ragnaglar the Unclean: */
{
REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_H_SUMMON, REW_SUMMON_M,
REW_SUMMON_M, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_WND,
REW_POLY_SLF, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_ABL, REW_SER_UNDE,
REW_CHAOS_WP, REW_GOOD_OBJ, REW_GOOD_OBJ, REW_GOOD_OBS, REW_GOOD_OBS
},

/* Cacodemon, Spawn of the Devil: */
{
REW_WRATH, REW_WRATH, REW_HURT_LOT, REW_PISS_OFF, REW_H_SUMMON,
REW_SUMMON_M, REW_IGNORE, REW_IGNORE, REW_DESTRUCT, REW_SER_UNDE,
REW_GENOCIDE, REW_MASS_GEN, REW_MASS_GEN, REW_DISPEL_C, REW_GOOD_OBJ,
REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_AUGM_ABL, REW_AUGM_ABL
},

/* Malia, Mistress of Disease: */
{
REW_WRATH, REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL,
REW_IGNORE, REW_IGNORE, REW_SER_UNDE, REW_DESTRUCT, REW_GENOCIDE,
REW_MASS_GEN, REW_MASS_GEN, REW_HEAL_FUL, REW_GAIN_ABL, REW_GAIN_ABL,
REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_AUGM_ABL, REW_AUGM_ABL
},

/* Pochnargo the Mutator: */
{
REW_TY_CURSE, REW_TY_CURSE, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_ABL,
REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND, REW_POLY_WND,
REW_GENOCIDE, REW_DISPEL_C, REW_GOOD_OBJ, REW_GOOD_OBJ, REW_SER_MONS,
REW_GAIN_ABL, REW_CHAOS_WP, REW_GAIN_EXP, REW_AUGM_ABL, REW_GOOD_OBS
},


/* Thanatar the Assassin: */
{
REW_WRATH, REW_TY_CURSE, REW_PISS_OFF, REW_H_SUMMON, REW_H_SUMMON,
REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_POLY_SLF,
REW_POLY_SLF, REW_SER_DEMO, REW_HEAL_FUL, REW_GAIN_ABL, REW_GAIN_ABL,
REW_CHAOS_WP, REW_DO_HAVOC, REW_GOOD_OBJ, REW_GREA_OBJ, REW_GREA_OBS
},

/* Gbaji the Deceiver: */
{
REW_TY_CURSE, REW_HURT_LOT, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL,
REW_SUMMON_M, REW_LOSE_EXP, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND,
REW_SER_UNDE, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_EXP, REW_GAIN_EXP,
REW_CHAOS_WP, REW_GOOD_OBJ, REW_GOOD_OBS, REW_GREA_OBS, REW_AUGM_ABL
},


/* Vivamort the Vampire Lord: */
{
REW_WRATH, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_EXP, REW_H_SUMMON,
REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF,
REW_POLY_SLF, REW_MASS_GEN, REW_SER_DEMO, REW_HEAL_FUL, REW_CHAOS_WP,
REW_CHAOS_WP, REW_GOOD_OBJ, REW_GAIN_EXP, REW_GREA_OBJ, REW_AUGM_ABL
},

/* Krarsht, the Hungry One: */
{
REW_WRATH, REW_TY_CURSE, REW_PISS_OFF, REW_CURSE_WP, REW_RUIN_ABL,
REW_IGNORE, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_WND,
REW_GOOD_OBJ, REW_GOOD_OBJ, REW_SER_MONS, REW_HEAL_FUL, REW_GAIN_EXP,
REW_GAIN_ABL, REW_CHAOS_WP, REW_GOOD_OBS, REW_GREA_OBJ, REW_AUGM_ABL
},

/* Wakboth, the Devil: */
{
REW_WRATH, REW_CURSE_AR, REW_CURSE_WP, REW_CURSE_WP, REW_CURSE_AR,
REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF,
REW_POLY_WND, REW_HEAL_FUL, REW_HEAL_FUL, REW_GAIN_EXP, REW_AUGM_ABL,
REW_GOOD_OBJ, REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_GREA_OBS
},

/* Bagog, the Scorpion Queen: */
{
REW_WRATH, REW_SER_DEMO, REW_CURSE_WP, REW_CURSE_AR, REW_LOSE_EXP,
REW_GAIN_ABL, REW_LOSE_ABL, REW_POLY_WND, REW_POLY_SLF, REW_IGNORE,
REW_DESTRUCT, REW_MASS_GEN, REW_CHAOS_WP, REW_GREA_OBJ, REW_HURT_LOT,
REW_AUGM_ABL, REW_RUIN_ABL, REW_H_SUMMON, REW_GREA_OBS, REW_AUGM_ABL
},

/* Gark the Calm: */
{
REW_WRATH, REW_HURT_LOT, REW_HURT_LOT, REW_H_SUMMON, REW_H_SUMMON,
REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_SER_MONS, REW_SER_DEMO,
REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GOOD_OBJ,
REW_CHAOS_WP, REW_GOOD_OBS, REW_GOOD_OBS, REW_GREA_OBJ, REW_GREA_OBS
},

/* Ikadz, Lord of Torture: */
{
REW_WRATH, REW_PISS_OFF, REW_PISS_OFF, REW_RUIN_ABL, REW_LOSE_ABL,
REW_LOSE_EXP, REW_IGNORE, REW_IGNORE, REW_POLY_WND, REW_SER_DEMO,
REW_POLY_SLF, REW_HEAL_FUL, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GAIN_EXP,
REW_GAIN_EXP, REW_CHAOS_WP, REW_GAIN_ABL, REW_GREA_OBJ, REW_AUGM_ABL
},

/* Kajabor: */
{
REW_WRATH, REW_PISS_OFF, REW_HURT_LOT, REW_RUIN_ABL, REW_LOSE_ABL,
REW_LOSE_EXP, REW_IGNORE, REW_IGNORE, REW_IGNORE, REW_POLY_SLF,
REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_GOOD_OBJ, REW_GAIN_ABL,
REW_GAIN_ABL, REW_SER_UNDE, REW_CHAOS_WP, REW_GREA_OBJ, REW_AUGM_ABL
},

/* Krjalk, the Traitor: */
{
REW_WRATH, REW_CURSE_WP, REW_CURSE_AR, REW_RUIN_ABL, REW_LOSE_ABL,
REW_LOSE_EXP, REW_IGNORE, REW_POLY_SLF, REW_POLY_SLF, REW_POLY_SLF,
REW_POLY_SLF, REW_POLY_WND, REW_HEAL_FUL, REW_CHAOS_WP, REW_GREA_OBJ,
REW_GAIN_ABL, REW_GAIN_ABL, REW_GAIN_EXP, REW_GAIN_EXP, REW_AUGM_ABL
},

/* Ompalam, Lord of Slavery: */
{
REW_WRATH, REW_HURT_LOT, REW_PISS_OFF, REW_LOSE_ABL, REW_LOSE_EXP,
REW_IGNORE,   REW_IGNORE,   REW_DISPEL_C, REW_DO_HAVOC, REW_DO_HAVOC,
REW_POLY_SLF, REW_POLY_SLF, REW_GAIN_EXP, REW_GAIN_ABL, REW_GAIN_ABL,
REW_SER_MONS, REW_GOOD_OBJ, REW_CHAOS_WP, REW_GREA_OBJ, REW_GOOD_OBS
}
};

martial_arts ma_blows[MAX_MA] =
{
#ifdef VERBOSE_MARTIAL_ARTS
{ "You punch %s.",                          1, 0, 1, 4, 0 },
{ "You kick %s.",                           2, 0, 1, 6, 0 },
{ "You strike %s.",                         3, 0, 1, 7, 0 },
{ "You hit %s with your knee.",             5, 5, 2, 3, MA_KNEE },
{ "You hit %s with your elbow.",            7, 5, 1, 8, 0 },
{ "You butt %s.",                           9, 10, 2, 5, 0 },
{ "You kick %s.",                           11, 10, 3, 4, MA_SLOW },
{ "You uppercut %s.",                       13, 12, 4, 4, 6 },
{ "You double-kick %s.",                    16, 15, 5, 4, 8 },
{ "You hit %s with a Cat's Claw.",          20, 20, 5, 5, 0 },
{ "You hit %s with a jump kick.",           25, 25, 5, 6, 10 },
{ "You hit %s with an Eagle's Claw.",       29, 25, 6, 6, 0 },
{ "You hit %s with a circle kick.",         33, 30, 6, 8, 10 },
{ "You hit %s with an Iron Fist.",          37, 35, 8, 8, 10 },
{ "You hit %s with a flying kick.",         41, 35, 8, 10, 12 },
{ "You hit %s with a Dragon Fist.",       45, 35, 10, 10, 16 },
{ "You hit %s with a Crushing Blow.",         48, 35, 10, 12, 18 },
#else
{ "You punch %s.",                          1, 0, 1, 4, 0 },
{ "You kick %s.",                           2, 0, 1, 6, 0 },
{ "You strike %s.",                         3, 0, 1, 7, 0 },
{ "You knee %s.",             5, 5, 2, 3, MA_KNEE },
{ "You hit %s.",            7, 5, 1, 8, 0 },
{ "You butt %s.",                           9, 10, 2, 5, 0 },
{ "You kick %s.",                           11, 10, 3, 4, MA_SLOW },
{ "You uppercut %s.",                       13, 12, 4, 4, 6 },
{ "You double-kick %s.",                    16, 15, 5, 4, 8 },
{ "You hit %s.",          20, 20, 5, 5, 0 },
{ "You kick %s.",           25, 25, 5, 6, 10 },
{ "You hit %s.",       29, 25, 6, 6, 0 },
{ "You kick %s.",         33, 30, 6, 8, 10 },
{ "You punch %s.",          37, 35, 8, 8, 10 },
{ "You kick %s.",         41, 35, 8, 10, 12 },
{ "You punch %s.",       45, 35, 10, 10, 16 },
{ "You punch %s.",       48, 35, 10, 12, 18 },
#endif
};


mindcraft_power mindcraft_powers[MAX_MINDCRAFT_POWERS] = {
/* Level gained,  cost,  %fail,  name */
{ 1,   1,  15, "Precognition" },       /* Det. monsters/traps */
{ 2,   1,  20, "Neural Blast" },     /* ~MM */
{ 3,   2,  25, "Minor Displacement" }, /* Phase/dimension door */
{ 7,   6,  35, "Major Displacement" }, /* Tele. Self / All */
{ 9,   7,  50, "Domination" },
{ 11,  7,  30, "Pulverise" },      /* Telekinetic "bolt" */
{ 13, 12,  50, "Character Armour" },   /* Psychic/physical defenses */
{ 15, 12,  60, "Psychometry" },
{ 18, 10,  45, "Mind Wave" },          /* Ball -> LOS */
{ 23, 15,  50, "Adrenaline Channeling" },
{ 25, 10,  40, "Psychic Drain" },      /* Convert enemy HP to mana */
{ 28, 20,  45, "Telekinetic Wave" },   /* Ball -> LOS */
};


/* Class Sub-Names based on realm */
cptr class_sub_name[MAX_CLASS][MAX_REALM+1] =
{
{
/* Warrior */
"Warrior", /* Default = No Magic */
"-",
"-",
"-",
"-",
"-",
"-",
"-",
"-"
},
{
/* Mage */
"Mage", /* Default */
"Mage", /* Life */
"Mage", /* Sorcery */
"Mage", /* Nature */
"Mage", /* Chaos */
"Mage", /* Death */
"Mage", /* Planar */
"Mage", /* Folk */
"Mage" /* Corporeal */
},
{
/* Priest */
"Priest", /* Default */
"Priest", /* Life */
"-",
"-",
"-",
"Cultist", /* Death */
"-",
"-",
"-"
},
{
/* Rogue */
"Rogue", /* Default */
"-",
"Burglar", /* Sorcery */
"-",
"-",
"Assassin", /* Death */
"Card Sharp", /* Planar */
"Thief", /* Folk */
"-"
},
{
/* Ranger */
"Ranger", /* Default */
"-",
"-",
"Ranger", /* Nature */
"-",
"-",
"-",
"-",
"-"
},
{
/* Paladin */
"Paladin", /* Default */
"Paladin", /* Life */
"-",
"-",
"-",
"Death Knight", /* Death */
"-",
"-",
"-"
},
{
/* Warrior-Mage */
"Warrior-Mage", /* Default */
"-",
"-",
"-",
"-",
"-",
"-",
"Warrior-Mage", /* Folk */
"-"
},
{
/* Diabolist */
"Diabolist", /* Default */
"-",
"-",
"-",
"Diabolist", /* Chaos */
"-",
"-",
"-",
"-"
},
{
/* Mystic */
"Mystic", /* Default */
"-",
"-",
"-",
"-",
"-",
"-",
"-",
"Mystic" /* Corporeal */
},
{
/* Mindcrafter */
"Mindcrafter", /* Default = No Magic */
"-",
"-",
"-",
"-",
"-",
"-",
"-",
"-"
},
{
/* High Mage */
"High-Mage", /* Default */
"Vivimancer", /* Life */
"Sorceror", /* Sorcery */
"Naturist", /* Nature */
"Warlock", /* Chaos */
"Necromancer", /* Death */
"Summoner", /* Planar */
"Hedge Wizard", /* Folk */
"Zen Master" /* Corporeal */
},
{
/* Druid */
"Druid", /* Default */
"-",
"-",
"Druid", /* Nature */
"-",
"-",
"-",
"-",
"-"
},
{
/* Demonologist */
"Demonologist", /* Default */
"-",
"-",
"-",
"Demonologist", /* Chaos */
"-",
"-",
"-",
"-"
},
};

