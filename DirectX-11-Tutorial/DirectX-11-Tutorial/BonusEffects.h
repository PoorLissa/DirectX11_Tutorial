#pragma once
#ifndef _BONUS_EFFECTS_H_
#define _BONUS_EFFECTS_H_

// ------------------------------------------------------------------------------------------------------------------------



// ����� �� ����� ���������� ���������, ������� ����� ������������� �� ������/��������.
// �������� ������� enum � ��������� �����, ����� ����� ����������� ������������ ������������� ���� Bonus::Effects::totalQty.
// ��� ���������� ������ ��� ���� ����������, �.�. ��������������� ���������� � ������ � enum �� ��������, � ������ ������� ����� Bonus ���� ������ Player
// ���� �� �������, �.�. ����� � ������ Bonus �������� ��������� �� Player, � ����� �� ��������������� ���������� Player �� ������� 0_o

// ??? - ������, ������ ���� � ���, ��� ����� ���� ���������� ������ ��� ������, � ����� ��� ��

class BonusEffects {

 public:
    // ��������� ��������� ������ ������ ���� _totalEffectsQty: ��� �� ������ ����� ������� ���������� ���������� ��������
    static enum Effects { HEAL = 0, FREEZE, SHIELD, FIRE_BULLETS, SLOW, _totalEffectsQty };
};

#endif
