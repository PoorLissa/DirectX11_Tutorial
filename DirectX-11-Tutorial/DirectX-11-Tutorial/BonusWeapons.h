#pragma once
#ifndef _BONUS_WEAPONS_H_
#define _BONUS_WEAPONS_H_

// ------------------------------------------------------------------------------------------------------------------------



// ����� �� ����� ������ ������, ���������� ������
class BonusWeapons {

 public:
    // PISTOL ������ ������ ���� ����� ������, �� ���� ��������� ������
    static enum Weapons { PISTOL = 100, RIFLE, SHOTGUN, ION_GUN, PLASMA_GUN, _lastWeapon, _totalWeaponsQty = _lastWeapon - PISTOL };
};

#endif
