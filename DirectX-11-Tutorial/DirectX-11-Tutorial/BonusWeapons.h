#pragma once
#ifndef _BONUS_WEAPONS_H_
#define _BONUS_WEAPONS_H_

// ------------------------------------------------------------------------------------------------------------------------



// Класс со всеми видами оружия, доступными игроку
class BonusWeapons {

 public:
    // PISTOL должен всегда быть самым первым, от него считаются номера
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, ION_GUN, PLASMA_GUN, _lastWeapon, _totalWeaponsQty = _lastWeapon - PISTOL };
};

#endif
