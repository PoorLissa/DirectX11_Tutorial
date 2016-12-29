#include "stdafx.h"
#include "Player.h"

// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������
void Player::Move(cfRef x, cfRef y, void *Param)
{
    static const float     divPIby180 = D3DX_PI / 180.0f;
    static unsigned short  bitFld = 0;

    // --- ������������ ����������� � ������� ������� ������ �� ������ ���� � ����������� �� ������� ������ ---
    {
	    _Step  = _Speed;
        bitFld = 0;

        // � ������ ������� ���� ������ ��������� ��� � sqrt(2) ���, ����� �������������� �������� ���� �������� ��������
        if( ( _Up && _Left ) || ( _Up && _Right ) || ( _Down && _Left ) || ( _Down && _Right ) )
            _Step *= 0.7071068f;

        // ������ ������� ������� �������� ���� ������ � bitFld, � ����� �� ��������� �������� ����� ���� ����� ��������� ������ ���� ��������
        if (  _Down ) { _Y += _Step; bitFld |= 1 << 3; }
        if (  _Left ) { _X -= _Step; bitFld |= 1 << 0; }
        if (    _Up ) { _Y -= _Step; bitFld |= 1 << 1; }
        if ( _Right ) { _X += _Step; bitFld |= 1 << 2; }

        if ( bitFld ) {

            switch( bitFld ) {
    
                case 1:  _Angle =   0.0f;  break;    // left
                case 2:  _Angle = 270.0f;  break;    // up
                case 4:  _Angle = 180.0f;  break;    // right
                case 8:  _Angle =  90.0f;  break;    // down
                case 3:  _Angle = 315.0f;  break;    // up + left
                case 6:  _Angle = 225.0f;  break;    // up + right
                case 9:  _Angle =  45.0f;  break;    // down + left
                case 12: _Angle = 135.0f;  break;    // down + right
            }

		    // ������� ������������ �������� ������ �� ������ ������� ���� � ��������� � �������
		    _Angle = (_Angle + _Angle0) * divPIby180;
        }
    }

    _weaponReady += _weaponReady < _weaponDelay ? 1 : 0;

    // --- ������������ �������� ������� ---
    {
        #define finalFactor     0.2f
        #define timeRemaining   EffectsCounters[effect]

        static unsigned int finalPart = EFFECT_DEFAULT_LENGTH * finalFactor;
        static unsigned int earlyPart = EFFECT_DEFAULT_LENGTH - finalPart;

        for (unsigned int effect = 0; effect < Bonus::Effects::_totalEffectsQty; effect++) {

            // ��������� ���������� ����� ������� �� ������ ����� �����
            if( timeRemaining ) {

                if( timeRemaining > 1 ) {

                    timeRemaining -= (effect == Bonus::Effects::SLOW) ? SLOW_EFFECT_FACTOR : 1;

                    // ���� � ������� ������ ������� ������ ����� 20% �� ��� ������� ������������, ����� ������� ������� ���� � ����:
                    // ??? ���� ��� ���� �������� - ���� ����� ����� ��������, ��� ����� �������� ������� ���������
                    if( timeRemaining > earlyPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // ������ ������������������ ������ �� �������� (appTimerInterval) �� (appTimerInterval * SLOW_EFFECT_FACTOR)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(EFFECT_DEFAULT_LENGTH - timeRemaining) / (finalPart);  // [0 ... 1];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor + 1);

                                break;
                            }
                        }
                    }

                    // ����� �� ��������� ������� �������� 20% �� ��� ������� ������������, ����� ������� ������� ����� �� ����:

                    if( timeRemaining < finalPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // ������ ������������������ ������ �� �������� (appTimerInterval * SLOW_EFFECT_FACTOR) �� (appTimerInterval)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(timeRemaining) / (finalPart);  // [1 ... 0];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor);

                                break;
                            }
                        }
                    }

                }
                else {
        
                    // ��������� �������, ����� ����� �� �������� �������
                    switch( effect )
                    {
                        case Bonus::Effects::SLOW:
                        {
                            _appTimer->reInitialize(appTimerInterval);
                            break;
                        }

                        case Bonus::Effects::FREEZE:
                        {
                            Monster::setFreeze(false);
                            break;
                        }

                        case Bonus::Effects::HEAL:
                        {
                            // ����� ������ ��������� �� �����
                            break;
                        }

                        case Bonus::Effects::SHIELD:
                        {
                            _Shielded = false;
                            break;
                        }

                        case Bonus::Effects::FIRE_BULLETS:
                        {
                            setBulletsType_Off(Player::BulletsType::FIRE);
                            setBulletsType_Off(Player::BulletsType::PIERCING);
                            setBulletsType_On (Player::BulletsType::NORMAL);

                            // ����������� ��� ����, ������� ��� �� ��������� �������
                            _bulletsType = _bulletsType_old;
                            _bulletsType_old = 12345;   // magic number, �������, ��� �� ��������, ������� �� �������� � ���� ���������� ������������ ��������
                            break;
                        }
                    }

                    timeRemaining = 0;
                }
            }
        }
    }

    #undef finalFactor
    #undef timeRemaining

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ������������� �������� ������ (������������ ������� �������� � �������������� � ������ ������, �� ������ ������ ����� �� ������ �� ������)
// ����� �� ������������� ������ ����� ������
void Player::setEffect(const unsigned int &effect)
{
    #define ThisEffectLength EffectsCounters[effect]

    // ��������� ������ ����� ������� �����, �������� �� ����������. ��������, SLOW. ������� ����������� ����� � ������ ����������.
    float effectLength = EFFECT_DEFAULT_LENGTH + 1;

    // �������� �����
    if( effect < BonusEffects::Effects::_totalEffectsQty ) {

        // ���� ������ ��� ���������, ������ ���������� ��� ������������
        if( ThisEffectLength > 0 ) {

            ThisEffectLength += effectLength;

        }
        else {

            // ���� ������ ��� �� ���������, ������������� ��� ������������ � �������� ��������������� �����
            ThisEffectLength = effectLength;

            switch( effect )
            {
                case Bonus::Effects::SLOW:
                {
                    //_appTimer->reInitialize(appTimerInterval * SLOW_EFFECT_FACTOR);
                    _appTimer->reInitialize(appTimerInterval + SLOW_EFFECT_FACTOR / 100);
                    break;
                }

                case Bonus::Effects::FREEZE:
                {
                    Monster::setFreeze(true);
                    break;
                }

                case Bonus::Effects::HEAL:
                {
                    _Health = _Health < _HealthMax - 100 ? _Health + 100 : _HealthMax;
                    break;
                }

                case Bonus::Effects::SHIELD:
                {
                    _Shielded = true;
                    break;
                }

                case Bonus::Effects::FIRE_BULLETS:
                {
                    // �������� ������� ��� ����
                    if( _bulletsType_old == 12345 )
                        _bulletsType_old = _bulletsType;

                    setBulletsType_Off(Player::BulletsType::NORMAL);
                    //setBulletsType_Off(Player::BulletsType::ION);
                    setBulletsType_On (Player::BulletsType::FIRE);
                    setBulletsType_On (Player::BulletsType::PIERCING);
                    break;
                }
            }
        }
    }

    // ����� ����� ������
    if( effect >= BonusWeapons::Weapons::PISTOL && effect < BonusWeapons::Weapons::_lastWeapon ) {
    
        switch( effect )
        {
            case BonusWeapons::Weapons::PISTOL:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 10;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);

                _weaponBulletSpeed  = 3;
#if 0
                _weaponDelay        = 3;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 50;

                //setBulletsType_On (Player::BulletsType::ION);
#endif
                break;
            }

            case BonusWeapons::Weapons::RIFLE:
            {
                _weaponDelay        = 5;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 20;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::SHOTGUN:
            {
                _weaponDelay        = 30;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 7;
                _weaponBulletSpread = 100;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::ION_GUN:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 7;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 5;

                setBulletsType_On (Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                setBulletsType_Off(Player::BulletsType::PLASMA);
                break;
            }

            case BonusWeapons::Weapons::PLASMA_GUN:
            {
                _weaponDelay        = 1;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::PLASMA);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
                break;
            }
        }

        _weaponReady = _weaponDelay;
    }

    #undef ThisEffectLength

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



void Player::spawnBullet_Normal(const int &mouseX, const int &mouseY, std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize,
                                    const int &wndPosX, const int &wndPosY)
{
    // �������� ���������� �� ������� �� ����� ������������ � ��������� ���������� � 500px, ��� �������� � ��������� ��������� ����

    short dx     = _X - mouseX;
    short dy     = _Y - mouseY;
    short dist   = sqrt(dx*dx + dy*dy);
    short Spread = dist * _weaponBulletSpread * 0.02;   // ����� �� ������ Spread � ����, ����� �� �� 500, � �� 50, � ��� ���� ����� �������� �������� ��� �� 10
    short halfSpread = Spread * 0.5f;

    // ��� ����������� ���� ��������� ��� ������ �������� &, �.�. � ������ ����� ���� ��������� �������� �����
    // � ����������� ���� �� �������� ���������� �������� �������

    if( _bulletsType & 1 << Player::BulletsType::ION )
    {
        if( _bulletsType & 1 << Player::BulletsType::FIRE ) {
            for (unsigned int i = 0; i < _weaponBurstQty; i++)
                bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                    mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                        _weaponBulletSpeed*5, Player::BulletsType::FIRE) );

            bulletListSize += _weaponBurstQty;
        }

        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new BulletIon(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::FIRE )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::FIRE) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::PIERCING )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::PIERCING) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    if( _bulletsType & 1 << Player::BulletsType::PLASMA )
    {
        for (unsigned int i = 0; i < _weaponBurstQty; i++)
            bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
                mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                    _weaponBulletSpeed, Player::BulletsType::PLASMA) );

        bulletListSize += _weaponBurstQty;

        return;
    }

    for (unsigned int i = 0; i < _weaponBurstQty; i++)
        bulletList->push_back( new Bullet(_X - wndPosX, _Y - wndPosY, 1.0f,
            mouseX - wndPosX + 0.1 * int(rand() % Spread - halfSpread), mouseY - wndPosY + 0.1 * int(rand() % Spread - halfSpread),
                _weaponBulletSpeed, Player::BulletsType::NORMAL) );

    bulletListSize += _weaponBurstQty;

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



void Player::spawnBullet_FourSides(std::list<gameObjectBase*> *bulletList, unsigned int &bulletListSize)
{
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X + 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X - 100, _Y, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y + 100, _weaponBulletSpeed, _bulletsType ) );
    bulletList->push_back( new Bullet(_X, _Y, 1.0f, _X, _Y - 100, _weaponBulletSpeed, _bulletsType ) );

    bulletListSize += 4;
}
// ------------------------------------------------------------------------------------------------------------------------
