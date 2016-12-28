#include "stdafx.h"
#include "Bonus.h"
#include "Player.h"



// ƒл€ бонуса рассчитываем оставшеес€ врем€ жизни, поворот, масштаб и взаимодействие с »гроком
void Bonus::Move(cfRef wndPosX, cfRef wndPosY, void *Param)
{
    //Player *player = static_cast<Player*>(Param);

    BonusParams *bpr = static_cast<BonusParams*>(Param);

    float playerX = bpr->player->getPosX();
    float playerY = bpr->player->getPosY();

    // ѕросчитываем действие эффекта Telekinetic: мышиного и гравитационного
    if( true ) {
    
        // mouse
        {
            int mouseX = bpr->mouseX;
            int mouseY = bpr->mouseY;

            if( abs(_X - mouseX + wndPosX) < 30 && abs(_Y - mouseY + wndPosY) < 30 ) {

                bpr->player->setEffect(_Effect);
                _Alive = false;
                return;
            }
        
        }

        // √равитационное прит€жение бонусов с карты
        {
            static int Const = 1e4;

            float dx = (_X - playerX + wndPosX);
            float dy = (_Y - playerY + wndPosY);

            float dist2 = (dx*dx + dy*dy);
            float dist1 = sqrt( dist2 );

            dist1 = Const / dist2 / dist1;

            _X -= dist1 * dx;
            _Y -= dist1 * dy;
        }
    }

    // Ѕонус вз€т (??? рассто€ние пока что выбрано методом научного тыка и считаетс€ не по окружности, а по квадрату)
    // ѕри этом бонус умирает, а игрок получает новый эффект
    if( abs(_X - playerX + wndPosX) < 30 && abs(_Y - playerY + wndPosY) < 30 ) {

        bpr->player->setEffect(_Effect);
        _Alive = false;
        return;
    }

    // ѕошевеливаем спрайт
    _AngleCounter += 0.01f;
    _ScaleCounter += 0.01f;
    _Angle = 0.5f * sin(_AngleCounter);

    // при по€влении бонуса он быстро вырастает из нул€, а позже плавно мен€ет размеры в малых границах
    if( _LifeTime > 140 )
        _Scale = _ScaleCounter < 0.33f ? _ScaleCounter * 3.0f :  1.0f + 0.1f * sin(_ScaleCounter);
    else
        _Scale = 1.0f;

    // когда врем€ жизни бонуса начинает истекать, два раза тикаем его и затем плавно уменьшаем размер почти до нул€
    if( _LifeTime <= 280 ) {

        if( !( _LifeTime > 140 && _LifeTime % 70 ) ) {
            _flashCounter = 5;
        }

        if( _flashCounter ) {
            _flashCounter--;
            _Scale += 0.075f;
        }

        if( _LifeTime < 110 ) {
            _ScaleModifier -= 0.0075f;
        }

        _Scale *= _ScaleModifier;
    }

    if( --_LifeTime <= 0 )
		_Alive = false;

    return;
}
