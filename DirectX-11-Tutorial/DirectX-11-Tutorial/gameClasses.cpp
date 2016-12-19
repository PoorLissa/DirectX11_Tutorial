#include "stdafx.h"
#include "gameClasses.h"

// �������������� ����������� ���������� � ���������� �������
bool Monster   :: _freezeEffect = false;
int  Bullet    :: _scrWidth     = 0;
int  Bullet    :: _scrHeight    = 0;
bool gameCells :: Single        = false;

ThreadPool* gameObjectBase::_thPool = nullptr; // ����������� ��� ������� ��������� �������� gameObjectBase

gameCells GameCells;

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
        #define finalFactor     0.2f;
        #define timeRemaining   EffectsCounters[effect]

        static unsigned int finalPart = EFFECT_DEFAULT_LENGTH * finalFactor;

        for (unsigned int effect = 0; effect < Bonus::Effects::_totalEffectsQty; effect++) {

            // ��������� ���������� ����� ������� �� ������ ����� �����
            if( timeRemaining ) {

                if( timeRemaining > 1 ) {

                    timeRemaining--;

                    // ����� �� ��������� ������� �������� 20% �� ��� ������� ������������, ����� ������� ������� ����� �� ����:

                    if( timeRemaining < finalPart ) {

                        switch( effect )
                        {
                            case Bonus::Effects::SLOW:
                            {
                                // ������ ������������������ ������ �� �������� (appTimerInterval * SLOW_EFFECT_FACTOR) �� (appTimerInterval * 1)
                                float factor = (SLOW_EFFECT_FACTOR - 1);

                                factor *= float(timeRemaining) / (finalPart);  // [1 ... 0];

                                _appTimer->reInitialize(appTimerInterval + appTimerInterval * factor);
                            }
                            break;
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
                        }
                        break;

                        case Bonus::Effects::FREEZE:
                        {
                            Monster::setFreeze(false);
                        }
                        break;

                        case Bonus::Effects::HEAL:
                        {
                            // ����� ������ ��������� �� �����
                        }
                        break;

                        case Bonus::Effects::SHIELD:
                        {
                            _Shielded = false;
                        }
                        break;

                        case Bonus::Effects::FIRE_BULLETS:
                        {
                            setBulletsType_Off(Player::BulletsType::FIRE);
                            setBulletsType_Off(Player::BulletsType::PIERCING);
                            setBulletsType_On (Player::BulletsType::NORMAL);

                            // ����������� ��� ����, ������� ��� �� ��������� �������
                            _bulletsType = _bulletsType_old;
                            _bulletsType_old = 12345;   // magic number, �������, ��� �� ��������, ������� �� �������� � ���� ���������� ������������ ��������
                        }
                        break;
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

        if( effect == BonusEffects::Effects::SLOW)
            effectLength /= SLOW_EFFECT_FACTOR;

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
                    _appTimer->reInitialize(appTimerInterval * SLOW_EFFECT_FACTOR);
                }
                break;

                case Bonus::Effects::FREEZE:
                {
                    Monster::setFreeze(true);
                }
                break;

                case Bonus::Effects::HEAL:
                {
                    _Health = _Health < _HealthMax - 100 ? _Health + 100 : _HealthMax;
                }
                break;

                case Bonus::Effects::SHIELD:
                {
                    _Shielded = true;
                }
                break;

                case Bonus::Effects::FIRE_BULLETS:
                {
                    // �������� ������� ��� ����
                    if( _bulletsType_old == 12345 )
                        _bulletsType_old = _bulletsType;

                    setBulletsType_Off(Player::BulletsType::NORMAL);
                    //setBulletsType_Off(Player::BulletsType::ION);
                    setBulletsType_On (Player::BulletsType::FIRE);
                    setBulletsType_On (Player::BulletsType::PIERCING);
                }
                break;
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
                //_weaponBulletSpeed  = 10;
                _weaponBulletSpeed  = 2;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 10;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::RIFLE:
            {
                _weaponDelay        = 10;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 20;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::SHOTGUN:
            {
                _weaponDelay        = 30;
                _weaponBulletSpeed  = 30;
                _weaponBurstQty     = 7;
                _weaponBulletSpread = 100;

                setBulletsType_On (Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;

            case BonusWeapons::Weapons::ION_GUN:
            {
                _weaponDelay        = 50;
                _weaponBulletSpeed  = 7;
                _weaponBurstQty     = 1;
                _weaponBulletSpread = 5;

                setBulletsType_On (Player::BulletsType::ION);
                setBulletsType_Off(Player::BulletsType::NORMAL);
                setBulletsType_Off(Player::BulletsType::PIERCING);
            }
            break;
        }

        _weaponReady = _weaponDelay;
    }

    #undef ThisEffectLength

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ����������� �������
void Monster::Move(cfRef x, cfRef y, void *Param)
{
#if 0
    _thPool->runAsync(&Monster::threadMove, this, x, y, Param);
#else
    if( !_freezeEffect ) {

        int currX = _X;
        int currY = _Y;

        olegType **olegArray = static_cast<olegType**>(Param);

        // ���� ������ ��������� � �������� ���������, ���� � ������������ �� ����������� ������� ��������� �� ���� � �������,
        // �.�. ���������� ������������� �� ������ �����
        if( currX >= 0 && currX < 800 && currY >= 0 && currY < 600 ) {

            std::vector<gameObjectBase*> *vec = &olegArray[currX][currY];

            for (int i = 0; i < vec->size(); i++) {

                gameObjectBase *ptr = vec->at(i);

                if( ptr == this ) {
                
                    vec->erase(vec->begin() + i);
                    break;
                }
            }
        }

	    float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) * 0.01f;

        _X += dX;
        _Y += dY;

	    animInterval1--;

	    if( animInterval1 < 0 ) {
		    animInterval1 = animInterval0;

		    animPhase++;

		    if(animPhase > animQty)
			    animPhase = 0;
	    }

        // ���������� ����� �������������� ������� � �����������
        currX = _X;
        currY = _Y;

        if( currX >= 0 && currX < 800 && currY >= 0 && currY < 600 ) {

            std::vector<gameObjectBase*> *vec = &olegArray[currX][currY];
            vec->push_back(this);
        }
    }
#endif
    return;
}

// ��������, ������� ������������ � ������-��������, �� �� ��������
void Monster::threadMove(cfRef x, cfRef y, void *Param)
{
    if( !_freezeEffect ) {

        BulletHelper *bltHelper = static_cast<BulletHelper*>(Param);

	    float dX = x - _X;
        float dY = y - _Y;
        float div_Speed_by_Dist = _Speed / sqrt(dX*dX + dY*dY);

        dX = div_Speed_by_Dist * dX * 0.1f * float(rand() % 200) * 0.01f;
        dY = div_Speed_by_Dist * dY * 0.1f * float(rand() % 200) * 0.01f;

        _X += dX;
        _Y += dY;

	    animInterval1--;

	    if( animInterval1 < 0 ) {
		    animInterval1 = animInterval0;

		    animPhase++;

		    if(animPhase > animQty)
			    animPhase = 0;
	    }

        //bltHelper->threadMove(this);
    }

    return;
}

// ------------------------------------------------------------------------------------------------------------------------

#define BULLET_BONUS_LIFE 0 // ��� ���������� ������

// ����������� ��� ����
Bullet::Bullet(cfRef x, cfRef y, cfRef scale, cfRef x_to, cfRef y_to, cfRef speed, UINT bulletType)
				: gameObjectBase(x, y, scale, 0.0f, speed, BULLET_DEFAULT_HEALTH + BULLET_BONUS_LIFE),
                    _squareSide(20),
                    _bulletType(bulletType)
{
	// �������� ������� ���� �� ����� ����, ����� ��� ������ ���� ��������� �� ����� �������� � ����� ������������
    // ����� ����� ���������� ���� ���� � ������, ������������ �������� ��� ������ �������� �������
    static const float divPIby180 = D3DX_PI / 180.0f;
    static const float div180byPI = 180.0f / D3DX_PI;

    _dX = x_to - x;
    _dY = y_to - y;

    if (_dX == 0.0f) {

        _Angle = _dY > 0.0f ? 180.0f : 0.0f;

    }
    else {

        _Angle = atan(_dY / _dX) * div180byPI;
        _Angle = _dX > 0.0f ? _Angle + 90.0f : _Angle + 270.0f;
    }

    _Angle = (270.0f - _Angle) * divPIby180;

    // �������� dX � dY
	// ����������� �������� ���:
	//	double Dist = sqrt((_x-X)*(_x-X) + (_y-Y)*(_y-Y));
	//	dX = Speed * (X-_x)/Dist;
	//	dY = Speed * (Y-_y)/Dist;
    // �� �� ������� �������� ����������:
    float Speed_Divided_By_Dist = _Speed / sqrt(_dX*_dX + _dY*_dY);
    _dX = Speed_Divided_By_Dist * _dX;
    _dY = Speed_Divided_By_Dist * _dY;

    // ������� ����� ������� �� ��������� �����, ����� ��� �� ��� ������� ������
	float initialMult = float( 1.0 + rand()%10 ) * 0.1f;
	_X += _dX * initialMult;
	_Y += _dY * initialMult;

    // ��������� �������� Piercing
    _piercing = bulletType & 1 << Player::BulletsType::PIERCING;

    // ������� ���� ���, ����� ��� ������ �� �� ������ ������, � �� ����� ������ ��� �����
    //static const int gunRadius = 35*2;
    static const int gunRadius = 20;
    _X0 = _X = _X - gunRadius * cos(_Angle);
    _Y0 = _Y = _Y + gunRadius * sin(_Angle);
}
// ------------------------------------------------------------------------------------------------------------------------



// ������� �������� ���� � ��������� �� � �������� (��������� ������)
// �� ���� �������� ������ ������� � ���������. ������������ ������������ ���� � ���������, � ��� �� ��� �������.
// ���������� ���� ���������� ������ ��� ������������� � ���������� �������� ������ � ����� �� ������������
void Bullet::threadMove_VECT(void *Param)
{
    float Rad = 20.0f;

    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// ������� ������� � ������ �����������, ��������� �� ���� ����� � ������ ��������
	// ��� (2 * 20000 ��������) � ������� 50-100 ����� ���� ������� �������� � 10 ��� (~33 ������ ~23)
    // ??? ����� ��� �������������������� � �������� ��������

    // ??? - ����� ��� ������� �������� �� ������� �����: ���� ������ ����� ������� �����. � ���� ����� ������, �� ��� ������� � ���� �� �������
    if( _dX > 0 ) {
        _squareX0 = int(_X);
        _squareX1 = int(_X + _dX);
    }
    else {
        _squareX1 = int(_X);
        _squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y);
        _squareY1 = int(_Y + _dY);
    }
    else {
        _squareY1 = int(_Y);
        _squareY0 = int(_Y + _dY);
    }

    _squareX0 -= _squareSide;
    _squareX1 += _squareSide;
    _squareY0 -= _squareSide;
    _squareY1 += _squareSide;


    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // �������� ������ � ��������� �� ������� ��������
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

#if defined useSorting

            // ���������� � ������� �������� ������� �� ������� ����������
            // � ����� ������, ���� �� �� ������������ � ����� ������� ������� ��������, �� ���������� ���� ��������
            if( _monsterX < _squareX0 ) {
                ++iter;
                continue;
            }
            // ... � ��� ������ �� ������ ������ ��� �������, �� ��������� ����, �.�. ��� ��������� ������� ����� ��� ������
            if( _monsterX > _squareX1 )
                break;

#endif

            // ������� ��������, ��������� �� ���� � ������ ����������� � �������, ����� �� ������� ����������� � ����������� ��� ������� ������� �� �����
            if( _squareX0 < _monsterX && _squareX1 > _monsterX && _squareY0 < _monsterY && _squareY1 > _monsterY )
            {
                // ���� ������ �������� ������������, ������������ ������ ������������
                if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                    (*iter)->setAlive(false);			// ������ ����

                    // ���� ������� ����� Piercing, �������� ����� ����� ���� �� �������. ���� ���, �� ���� ������� ����� ������� �� ���������.

                    _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                    // ���� ����� ����� ���� �������, �� ���� ���������:
					if( !_Health ) {

						this->_Alive = false;

						_dX = _dY = 0.0;            // ������������� ����
						_X = (float)_monsterX;      // ��������� ���� � ����� �������
						_Y = (float)_monsterY;

						return;
					}
                }

            } // if-square...

            ++iter;
        }
    }

    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // ���� ���� � ������
        return;
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



void Bullet::threadMove_Oleg(void *Param)
{
    float Rad = 20.0f;

    olegType **olegArray = static_cast<olegType **>(Param);
/*
    if( _dX > 0 ) {
        _squareX0 = int(_X) - 2;
        _squareX1 = int(_X + _dX + _squareSide);
    }
    else {
        _squareX1 = int(_X) + 2;
        _squareX0 = int(_X + _dX - _squareSide);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y) - 2;
        _squareY1 = int(_Y + _dY + _squareSide);
    }
    else {
        _squareY1 = int(_Y) + 2;
        _squareY0 = int(_Y + _dY - _squareSide);
    }
*/

    if( _dX > 0 ) {
        _squareX0 = int(_X) - 2;
        _squareX1 = int(_X + _dX + _squareSide);
    }
    else {
        _squareX1 = int(_X) + 2;
        _squareX0 = int(_X + _dX - _squareSide);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y) - 2;
        _squareY1 = int(_Y + _dY + _squareSide);
    }
    else {
        _squareY1 = int(_Y) + 2;
        _squareY0 = int(_Y + _dY - _squareSide);
    }

    if( _squareX0 >= 0 && _squareX1 < olegMaxX && _squareY0 >= 0 && _squareY1 < olegMaxY )
//lalala
    // ������� �� ���� ������� ������������, ������� ��������� � �������������� ������ ���� � �������� ��������� � ��������
    for (int i = _squareX0; i < _squareX1; i++) {
    
        for (int j = _squareY0; j < _squareY1; j++) {

            std::vector<gameObjectBase*> *vec = &olegArray[i][j];

            for (int monster = 0; monster < vec->size(); monster++) {
            
                _monsterX = vec->at(monster)->getPosX();
                _monsterY = vec->at(monster)->getPosY();

                float dx = _X - _monsterX;
                float dy = _Y - _monsterY;
                float dist = sqrt(dx*dx + dy*dy);

                if( dist < Rad || commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad) ) {

                    vec->at(monster)->setAlive(false);  // ������ ����

                    // ���� ������� ����� Piercing, �������� ����� ����� ���� �� �������. ���� ���, �� ���� ������� ����� ������� �� ���������.

                    _Health = _bulletType & 1 << Player::BulletsType::PIERCING ? _Health-- : 0;

                    // ���� ����� ����� ���� �������, �� ���� ���������:
					if( !_Health ) {

						this->_Alive = false;

						_dX = _dY = 0.0;            // ������������� ����
						_X = (float)_monsterX;      // ��������� ���� � ����� �������
						_Y = (float)_monsterY;

						return;
					}
                }
            }
        }
    }

    // big if

    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight )
        this->_Alive = false;   // ���� ���� � ������

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ������ ���� ������������ �����, ������ ��� � ��� ���� �) ������, �) ����� ��� ���������
void BulletIon::threadMove(void *Param)
{
    float Rad = 20.0f;

    std::vector< std::list<gameObjectBase*>* > *VEC = static_cast< std::vector< std::list<gameObjectBase*>*>* >(Param);

	// ������� ������� � ������ �����������, ��������� �� ���� ����� � ������ ��������

    if( _dX > 0 ) {
        _squareX0 = int(_X);
        _squareX1 = int(_X + _dX);
    }
    else {
        _squareX1 = int(_X);
        _squareX0 = int(_X + _dX);
    }

    if( _dY > 0 ) {
        _squareY0 = int(_Y);
        _squareY1 = int(_Y + _dY);
    }
    else {
        _squareY1 = int(_Y);
        _squareY0 = int(_Y + _dY);
    }

    // ���������� ������ ������� ������
    if( _bulletType == Player::BulletsType::ION_EXPLOSION ) {

        _Health++;

        // 15 ����� - �������������� ������ ��� ���������� ������, ������ �������� �� ���� ������ (����, � �������� ������, � �������� ������)
        if( _Health <= 15 ) {   
            _Scale = _Health;
            Rad    = _Health * 5;  // bullet sprite texture size / 2 (as in the _gameShader_Bullet.vs file)

            // ��� ������� ������ ������������ ���� ����������� ������ �������
            int SquareSize = Rad + 10;

            _squareX0 -= SquareSize;
            _squareX1 += SquareSize;
            _squareY0 -= SquareSize;
            _squareY1 += SquareSize;
        }
        else
            this->_Alive = false;

    }
    else {

        _squareX0 -= _squareSide;
        _squareX1 += _squareSide;
        _squareY0 -= _squareSide;
        _squareY1 += _squareSide;

    }

    for (unsigned int lst = 0; lst < VEC->size(); lst++) {
        
        std::list<gameObjectBase*> *list = VEC->at(lst);

        // �������� ������ � ��������� �� ������� ��������
        std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
        while (iter != end) {

            _monsterX = (int)(*iter)->getPosX();
            _monsterY = (int)(*iter)->getPosY();

            // ������� ��������, ��������� �� ���� � ������ ����������� � �������, ����� �� ������� ����������� � ����������� ��� ������� ������� �� �����
            if( _squareX0 < _monsterX && _squareX1 > _monsterX && _squareY0 < _monsterY && _squareY1 > _monsterY )
            {
                switch( _bulletType )
                {
                    // ��� ������ ���� (� ������� ���� ������): �������, ��� ���� �������� � �������, ���� �� ����������� ����� ���������� ����������,
                    // ������ ������� ����� ����� �������� ������� � ����
                    case Player::BulletsType::ION:
                    {
                        if( commonSectionCircle(_X, _Y, _X + _dX, _Y + _dY, _monsterX, _monsterY, Rad + 25) )
                        {
                            // ��� ���������: ������������� ����, ��������� �� � ����� ������� � �������� �� ��� �� ION_EXPLOSION.
                            // ����� � �������� ����� ����� (� � ��� � ������ ������), ���� �� ��������� ��������� ��������� ��������, ����� ������� ������ ������
                            _bulletType = Player::BulletsType::ION_EXPLOSION;
                            
					        _dX = _dY = 0.0;
					        _X = (float)_monsterX;
					        _Y = (float)_monsterY;
                            _Health = 3;

                            return;
                        }
                    }
                    break;

                    // �������� ��� ������� ������. �� �������� - ��������� ����������� ��������� �������, ����������� ������ �������, � ���������� ������
                    case Player::BulletsType::ION_EXPLOSION:
                    {
                        // ���� ������� ������� �������, ����������� ��� ����� (� ���� ��� ������ ������� � ������ �����)
                        // ����� �����, ��� ������ ������ ������������� �� ���������� ���������� �������, ������� ����� ����������� ���������:
                        // �� �������, ������� ����� � ������ ������, �������� ��� ������� ������ ������ � ������� ������� ��������� ����, ��� ��, ��� �� ���������
                        if( commonSectionCircle(_monsterX-10, _monsterY-10, _monsterX+10, _monsterY+10, _X, _Y, Rad) )
                            (*iter)->setAlive(false);
                    }
                    break;
                }

            }

            ++iter;
        }
    }

    // ���������� ���� ������, ���� �� � ���� �� ������
    _X += _dX;
    _Y += _dY;

    if ( _X < -50 || _X > _scrWidth || _Y < -50 || _Y > _scrHeight ) {
        _dX = _dY = 0.0;
        this->_Alive = false;   // ���� ���� � ������
        return;
    }

    return;
}
// ------------------------------------------------------------------------------------------------------------------------



// ����������� ������� � ����������� ��� ����������� ��������� ���� � �������
// http://www.cyberforum.ru/cpp-beginners/thread853799.html
bool Bullet::commonSectionCircle(float x1, float y1, float x2, float y2, const int &xCirc, const int &yCirc, const float &Rad)
{
    x1 -= xCirc;
    y1 -= yCirc;
    x2 -= xCirc;
    y2 -= yCirc;

    dx = x2 - x1;
    dy = y2 - y1;

    // ���������� ������������ ����������� ��������� �� ����������� ������ � ����������.
    // ���� �� ������� [0..1] ���� ������������� ��������, ������ ������� ���������� ����������
    a = dx*dx + dy*dy;
    b = 2.0f * (x1*dx + y1*dy);
    c = x1*x1 + y1*y1 - Rad*Rad;

    // � ������ ���������, ���� �� �� ������� [0..1] �������
    if (-b < 0)
        return c < 0;

    if (-b < (2.0 * a))
        return (4.0 * a*c - b*b) < 0;

    return (a + b + c) < 0;
}
// ------------------------------------------------------------------------------------------------------------------------
