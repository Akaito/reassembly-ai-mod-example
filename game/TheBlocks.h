
#ifndef _MSC_VER
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include "Colors.h"
#include "RGB.h"

//#define SQRT2 "sqrt2"
// #define SQRT2 "\u221A(2)"
#define SQRT2 "\xE2\x88\x9A(2)"
//#define ONE_HALF "1/2"
// #define ONE_HALF "\u00BD"
#define ONE_HALF "\xC2\xBD"
//#define CROSS " by "
//#define CROSS "\u00D7"
#define CROSS "\xC3\x97"

enum Colors {
    neonblue     = 0x6fecff,
    lightblue    = 0x009aff,
    medblue      = 0x0033ad,
    darkblue     = 0x001851,
    neonred      = 0xff0000,
    lightred     = 0xe33434,
    medred       = 0xa01d10,
    darkred      = 0x800000,
    lightorange  = 0xff7100,
    darkorange   = 0xc75800,
    neongreen    = 0x25ff00,
    lightgreen   = 0x4fdb54,
    medgreen     = 0x03b800,
    darkgreen    = 0x185a26,
    lightgrey    = 0xd8d8d8,
    medgrey      = 0x5c5c5c,
    darkgrey     = 0x383838,
    medpurple    = 0x7B00FF,
    darkpurple   = 0x2f0056,
};

static void factionTwo()
{
    const uint fillbase     = 0x202020;
    const uint fillbase1    = 0x404040;
    const uint fillthrust   = 0x0c4e61;
    const uint fillthrust1  = 0x2c3e41;
    const uint linethrust   = 0x70a000;
    const uint thrustColor  = 0x95aeac;
    const uint thrustColor1 = 0x75ceac;
    const uint fillgen      = 0x2e535e;
    const uint fillgen1     = 0x4e535e;
    const uint line         = 0xaef300;
    const uint fillshield   = 0x769a95;
    const uint fillcommand  = 0x069a95;
    const uint fillcommand1 = 0x066a95;
    const uint gun          = 0x79a82d; // dturret
    const uint gun01        = 0x69989d; // dturret
    const uint gun1         = 0x104a99; // standard
    const uint gun2         = 0x99a80d; // shotgun
    const uint gun21        = 0x878801; // shotgun
    const uint gun3         = 0x061a65; // sniper
    const uint gun31        = 0x063a45; // sniper
    const uint gun3line     = 0x095ac5; // sniperr
    const uint missile      = 0x5b5b5b;
    const uint missileline  = 0x3b9b3b;
    const uint laser        = 0x004022;
    const uint laser1       = 0x407000;
    const uint laser2       = 0x80e040;
    const uint projectile   = 0xECFE00;
        
    { SerialBlock &b = materializeBlock(NULL, 200, 2, fillcommand, line, SHAPE_COMMAND,
                                        Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Command Module");
        b.fillColor1              = fillcommand1;
        b.generatorCapacityPerSec = 100;
        b.density                 = 2.f * kComponentDensity;
        b.blurb                   = _("The core of every ship"); }

    SerialBlock &separator = materializeBlock(NULL, 201, 2, fillbase, line,
                                              SHAPE_OCTAGON, 0);
    {   SerialBlock &b = separator;
        b.fillColor1   = fillbase1;
        b.name         = _("Resource Containment Unit");
        b.capacity     = 50.f;
        b.blurb        = _("Resource storage unit");}

    SerialBlock &lightThruster = materializeBlock(NULL, 202, 2, fillthrust, linethrust,
                                                  SHAPE_DISH_THRUSTER, Block::THRUSTER);
    {   SerialBlock &b  = lightThruster;
        b.fillColor1    = fillthrust1;
        b.name          = _("Thruster");
        b.thrusterColor = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.thrusterForce = 20000; 
        b.scale    = 2;
        b.blurb         = _("Produces a small amount of thrust");}

    {   SerialBlock &b  = materializeBlock(&lightThruster, 203, 2);
        b.name          = _("Thruster");
        b.thrusterForce = 40000;
        b.scale    = 3;
        b.blurb         = _("Provides a medium amount of thrust");}

    {   SerialBlock &b  = materializeBlock(&lightThruster, 204, 2);
        b.name          = _("Thruster");
        b.thrusterForce = 80000;
        b.scale    = 4;
        b.blurb         = _("Provides a large amount of thrust");}
    
    SerialBlock &lightGenerator = materializeBlock(NULL, 205, 2, fillgen, line,
                                                   SHAPE_SQUARE, Block::GENERATOR);
    {   SerialBlock &b            = lightGenerator;
        b.fillColor1              = fillgen1;
        b.name                    = _("Generator");
        b.generatorCapacityPerSec = 120;
        b.blurb                   = _("Produces energy to power "
                                    "a ship's weapons. More "
                                      "generators, more energy");}
    
    {   SerialBlock &b            = materializeBlock(&lightGenerator, 207, 2);
        b.name                    = _("Generator");
        b.generatorCapacityPerSec = 500;
        b.scale              = 3;
        b.blurb                   = _("Produces energy to power "
                                    "a ship's weapons. More "
                                      "generators, more energy");}

    {   SerialBlock &b = materializeBlock(NULL, 265, 2, fillgen, line, SHAPE_OCTAGON, 
                                          Block::PHOTOSYNTH);
        b.name             = _("Solar Panel");
        b.scale       = 4;
        b.density          = 0.1f * kComponentDensity;
        b.capacity         = 400;
        b.durability       = 0.1f;
        b.fillColor1       = fillcommand;
        b.photosynthPerSec = 0.2; }
    
    {   SerialBlock &b = materializeBlock(&separator, 208, 2);
        b.scale   = 2;
        b.capacity     = 200.f;
        b.blurb        = _("Resource storage unit");}

    
    {   SerialBlock &b = materializeBlock(&separator, 213, 2);
        b.name         = _("Adapter");
        b.shape   = SHAPE_ADAPTER;
        b.scale   = 1;
        b.blurb        = _("A basic building block");}

    {   SerialBlock &b = materializeBlock(&separator, 214, 2);
        b.name         = _("Adapter");
        b.shape   = SHAPE_ADAPTER;
        b.scale   = 2;
        b.blurb        = _("A basic building block");}


    for (int i=1; i<6; i++) {
        SerialBlock &b = materializeBlock(NULL, (i == 2) ? 217 : 275+i, 2, fillbase, line,
                                          SHAPE_RECT, 0);
        b.name       = _("Armor");
        b.blurb  = string(_("Extra durable")) + ", " + 
                   ((i == 1) ? "1" CROSS "(1 - 1/" SQRT2 ") " :
                    (i == 2) ? "1" CROSS ONE_HALF " " :
                    (i == 3) ? "1" CROSS "1/" SQRT2 " " :
                    (i == 4) ? "2" CROSS "(2 - 2/" SQRT2 ") " :
                    (i == 5) ? "2" CROSS "2/" SQRT2 " " : "") + _("units");
        b.fillColor1 = fillbase1;
        b.shape = SHAPE_RECT;
        b.scale = i;
        b.durability = 4;
        b.density    = 3.f * kComponentDensity;
    }
    
    {   SerialBlock &b          = materializeBlock(NULL, 218, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                  = _("RK-12 Standard");
        b.scale            = 1;
        b.cannon->color         = projectile;
        b.cannon->range         = 500;
        b.cannon->damage        = 16;
        b.cannon->roundsPerSec  = 6;
        b.cannon->muzzleVel     = 800;
        b.blurb                 = _("Basic light cannon. Its consistent "
                                  "rate of fire and low energy use make "
                                    "it a reliable weapon");}
    
    {   SerialBlock &b = materializeBlock(NULL, 219, 2, fillbase, line,
                                              SHAPE_SQUARE, 0);
        b.fillColor1 = fillbase1;
        b.name  = _("Hull");}
    
    /* REFERENCE FOR BLOCK DEFINITION:
    static SerialBlock &materializeBlock(const SerialBlock *parent, uint ident, uint faction, uint fillColor, uint lineColor,
    BlockShape shape, uint features)*/
    
    {   SerialBlock &b          = materializeBlock(NULL, 220, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                  = _("MGVD-4 Bolshevik");
        b.scale            = 2;
        b.cannon->color         = projectile;
        b.cannon->range         = 1000;
        b.cannon->damage        = 200;
        b.cannon->explosive     = EExplosive::FINAL;
        b.cannon->explodeRadius = 50;
        b.cannon->roundsPerSec  = 2;
        b.cannon->muzzleVel     = 400;
        b.blurb                 = _("Fires numerous but slow "
                                  "explosive rounds, akin "
                                    "to a grenade launcher");}
    
    {   SerialBlock &b          = materializeBlock(NULL, 221, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                  = _("AF-75 Spitter");
        b.scale            = 2;
        b.cannon->color         = projectile;
        b.cannon->range         = 1200;
        b.cannon->damage        = 50;
        b.cannon->roundsPerSec  = 3;
        b.cannon->burstyness    = 0.7;
        b.cannon->roundsPerBurst= 3;
        b.cannon->muzzleVel     = 600;
        b.blurb                 = _("A powerful burst-fire "
                                  "turret that excels at "
                                    "medium range");}
    
    
    {   SerialBlock &b           = materializeBlock(NULL, 223, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("Minigun");
        b.scale             = 1;
        b.cannon->color          = projectile;
        b.cannon->range          = 900;
        b.cannon->damage         = 20;
        b.cannon->roundsPerSec   = 10;
        b.cannon->muzzleVel      = 1100;
        b.cannon->spread         = 0.1;
        b.cannon->burstyness     = 0; }
    
    {   SerialBlock &b         = materializeBlock(NULL, 224, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                 = _("Swatter");
        b.fillColor1           = gun01;
        b.scale           = 1;
        b.turretSpeed          = 20.f;
        b.cannon->color        = projectile;
        b.cannon->range        = 500;
        b.cannon->damage       = 10;
        b.cannon->roundsPerSec = 12;
        b.cannon->muzzleVel    = 1000;
        b.cannon->spread       = 0.15;
        b.cannon->burstyness   = 0; }

    {   SerialBlock &b           = materializeBlock(NULL, 226, 2, gun2, line, SHAPE_CANNON2, Block::CANNON);
        b.name                   = _("SG-9 Scattergun");
        b.fillColor1             = gun21;
        b.scale             = 2;
        b.cannon->color          = projectile;
        b.cannon->range          = 500;
        b.cannon->damage         = 25;
        b.cannon->roundsPerSec   = 30;
        b.cannon->muzzleVel      = 1000;
        b.cannon->spread         = 0.15;
        b.cannon->burstyness     = 0.95;
        b.cannon->roundsPerBurst = 15;
        b.blurb                  = _("Anti-ship shotgun"); }
    
    {   SerialBlock &b           = materializeBlock(NULL, 227, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("HC2");
        b.scale             = 3;
        b.cannon->color          = projectile;
        b.cannon->range          = 3000;
        b.cannon->damage         = 210;
        b.cannon->roundsPerSec   = 0.5;
        b.cannon->muzzleVel      = 1100;
        b.cannon->spread         = 0;
        b.cannon->burstyness     = 0; }
    
    {   SerialBlock &b           = materializeBlock(NULL, 228, 2, gun3, gun3line, SHAPE_CANNON, Block::CANNON);
        b.name                   = _("EXR-1 Railgun");
        b.fillColor1             = gun31;
        b.scale             = 3;
        b.cannon->color          = projectile;
        b.cannon->range          = 6000;
        b.cannon->damage         = 240;
        b.cannon->explosive      = EExplosive::FINAL;
        b.cannon->explodeRadius  = 10;
        b.cannon->roundsPerSec   = 0.3;
        b.cannon->muzzleVel      = 1500;
        b.cannon->spread         = 0;
        b.cannon->burstyness     = 0;
        b.blurb                  = _("Unmatched in range, this "
                                   "sniper rifle will always "
                                     "give you the first shot");}

        {   SerialBlock &b           = materializeBlock(NULL, 229, 2, laser2, line,
                                                        SHAPE_OCTAGON, Block::LASER|Block::TURRET);
        b.name                     = _("MED Beam");
        b.fillColor1               = laser1;
        b.scale                    = 2;
        b.laser->color             = 0x40000000|laser2;
        b.laser->range             = 1000;
        b.laser->damage            = -300;
        b.laser->width             = 10;}
    
    
    
    {   SerialBlock &b           = materializeBlock(NULL, 231, 2, gun, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                   = _("Phalanx");
        b.fillColor1             = gun01;
        b.scale             = 1;
        b.turretSpeed            = 20.f;
        b.cannon->color          = projectile;
        b.cannon->range          = 1300;
        b.cannon->damage         = 5;
        b.cannon->roundsPerSec   = 10;
        b.cannon->muzzleVel      = 1200;
        b.cannon->spread         = 0.05;
        b.cannon->burstyness     = 0; }
    
    {   SerialBlock &b           = materializeBlock(NULL, 232, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("MC3");
        b.scale             = 2;
        b.cannon->color          = projectile;
        b.cannon->range          = 1400;
        b.cannon->damage         = 100;
        b.cannon->roundsPerSec   = 1.5;
        b.cannon->muzzleVel      = 1200;
        b.cannon->spread         = 0;
        b.cannon->burstyness     = 0; }
    
    {   SerialBlock &b           = materializeBlock(NULL, 234, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("HC3");
        b.scale             = 3;
        b.cannon->color          = projectile;
        b.cannon->range          = 3200;
        b.cannon->damage         = 300;
        b.cannon->roundsPerSec   = 0.6;
        b.cannon->muzzleVel      = 1200;
        b.cannon->spread         = 0;
        b.cannon->burstyness     = 0; }
    
    {   SerialBlock &b           = materializeBlock(NULL, 237, 2, gun1, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("MLS Avenger");
        b.scale             = 1;
        b.cannon->color          = projectile;
        b.cannon->range          = 800;
        b.cannon->damage         = 15;
        b.cannon->roundsPerSec   = 15;
        b.cannon->muzzleVel      = 1700;
        b.cannon->spread         = 0.01;
        b.cannon->burstyness     = 0;
        b.blurb                  = _("Very high rate of fire, medium range");}
    
    {   SerialBlock &b           = materializeBlock(NULL, 238, 2, gun, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                   = _("MLS Anti-Missile");
        b.scale             = 1;
        b.turretSpeed            = 20.f;
        b.cannon->color          = projectile;
        b.cannon->range          = 600;
        b.cannon->damage         = 10;
        b.cannon->explosive      = EExplosive::PROXIMITY;
        b.cannon->explodeRadius  = 20.f;
        b.cannon->roundsPerSec   = 4;
        b.cannon->muzzleVel      = 1800;
        b.cannon->spread         = 0.01;
        b.cannon->burstyness     = 0;
        b.blurb                  = _("Close in weapon system");}
    
    
    {   SerialBlock &b           = materializeBlock(NULL, 242, 2, gun3, gun3line, SHAPE_CANNON, Block::CANNON);
        b.name                   = _("Longbow");
        b.fillColor1             = gun31;
        b.scale             = 3;
        b.cannon->color          = projectile;
        b.cannon->range          = 5000;
        b.cannon->damage         = 320;
        b.cannon->explosive      = EExplosive::FINAL;
        b.cannon->explodeRadius  = 30;
        b.cannon->roundsPerSec   = 1.f;
        b.cannon->muzzleVel      = 2200;
        b.cannon->spread         = 0;
        b.cannon->burstyness     = 0; }

    {   SerialBlock &b = materializeBlock(NULL, 245, 2, missile, line,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                           = _("ZLM Sparrow");
        b.launcherPower                  = 50;
        b.launcherOutSpeed               = 50.f; 
        b.replicateTime = 0.7;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterForce = 1500.f;
        b.replicateBlock->torquerTorque = 2000;
        b.replicateBlock->durability     = 0.1;
        b.replicateBlock->lifetime      = 5;
        b.replicateBlock->explodeDamage = 40;
        b.replicateBlock->explodeRadius = 20;
        b.replicateBlock->fillColor     = projectile;
        b.replicateBlock->lineColor     = missileline;
        b.blurb                          = _("Who doesn't like showering "
                                             "a target in missiles?");}
    
    
    {   SerialBlock &b   = materializeBlock(NULL, 247, 2, fillshield, line, SHAPE_OCTAGON, Block::SHIELD);
        b.name           = _("Shield");
        b.shield->radius   = 50;
        b.shield->strength = 300;
        b.shield->regen    = 200;
        b.scale     = 1;
        b.density           = 5.f * kComponentDensity; 
        b.blurb          = _("Low strength and limited "
                           "area is balanced by a fast "
                             "recharge time");}
    
    {SerialBlock &b = materializeBlock(NULL, 248, 2, fillbase, line, SHAPE_RIGHT_TRI, Block::ZERO);
        b.name        = _("Hull");
        b.fillColor1 = fillbase1;
        b.blurb        = _("A basic building block");}
    
    {   SerialBlock &b = materializeBlock(NULL, 249, 2, missile, line,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                           = _("MM1");
        b.launcherPower                  = 50;
        b.launcherOutSpeed               = 50.f; 
        b.scale                     = 2;
        b.replicateTime = 2.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->thrusterForce = 6000.f;
        b.replicateBlock->torquerTorque = 6000;
        b.replicateBlock->durability     = 1.f;
        b.replicateBlock->lifetime      = 8;
        b.replicateBlock->explodeDamage = 80;
        b.replicateBlock->explodeRadius = 50;
        b.replicateBlock->fillColor     = projectile;
        b.replicateBlock->lineColor     = missileline;
        b.replicateBlock->scale    = 2;}
    
    {   SerialBlock &b = materializeBlock(NULL, 250, 2, missile, line,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.name                           = _("HM1");
        b.launcherPower                  = 80;
        b.launcherOutSpeed               = 50.f; 
        b.scale                     = 2;
        b.replicateTime = 1.5;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->thrusterForce = 6000.f;
        b.replicateBlock->torquerTorque = 6000.f;
        b.replicateBlock->durability     = 0.5f;
        b.replicateBlock->lifetime      = 12;
        b.replicateBlock->explodeDamage = 120;
        b.replicateBlock->explodeRadius = 90;
        b.replicateBlock->fillColor     = projectile;
        b.replicateBlock->lineColor     = missileline;
        b.replicateBlock->scale    = 3;}
    
    {   SerialBlock &b           = materializeBlock(NULL, 251, 2, laser, line,
                                                    SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.name                   = _("Arkas PRP-1");
        b.fillColor1             = laser1;
        b.laser->color             = 0x80000000|projectile;
        b.laser->range             = 1000;
        b.laser->damage            = 75;
        b.laser->width             = 3;
        b.laser->pulsesPerSec      = 3;
        b.laser->pulseAvailability = 0.5f;
        b.blurb                  = _("This small laser cannon "
                                   "is capable of taking out "
                                     "small targets in seconds");}
    
    {   SerialBlock &b           = materializeBlock(NULL, 252, 2, laser, line,
                                                    SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.name                   = _("Arkas PRP-2");
        b.fillColor1             = laser1;
        b.laser->color             = 0x80000000|projectile;
        b.laser->range             = 900;
        b.laser->damage            = 125;
        b.laser->width             = 5;
        b.laser->pulsesPerSec      = 3;
        b.laser->pulseAvailability = 0.75f;
        b.scale             = 2;
        b.blurb                  = _("The instantaneous nature "
                                   "of lasers mean that damage "
                                     "is delivered before all else");}
    
    {   SerialBlock &b           = materializeBlock(NULL, 253, 2, laser, line,
                                                    SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.name                   = _("Arkas PRP-3");
        b.fillColor1             = laser1;
        b.laser->color             = 0x80000000|projectile;
        b.laser->range             = 800;
        b.laser->damage            = 250;
        b.laser->width             = 7;
        b.laser->pulsesPerSec      = 2;
        b.laser->pulseAvailability = 0.85f;
        b.scale             = 3;
        b.blurb                  = _("Need to burn through a "
                                   "battleship without breaking "
                                     "a sweat? Try this");}
    
    {   SerialBlock &b   = materializeBlock(NULL, 254, 2, fillshield, line, SHAPE_OCTAGON, Block::SHIELD);
        b.name           = _("Shield");
        b.shield->radius   = 100;
        b.shield->strength = 1000;
        b.shield->regen    = 200;
        b.scale     = 2; 
        b.blurb          = _("This mid-size generator is "
                             "versatile and effective");}
    
    {   SerialBlock &b   = materializeBlock(NULL, 255, 2, fillshield, line, SHAPE_OCTAGON, Block::SHIELD);
        b.name           = _("Shield");
        b.shield->radius   = 175;
        b.shield->strength = 4000;
        b.shield->regen    = 600;
        b.scale     = 3; 
        b.blurb          = _("When you've got a lot of "
                           "hardware to protect, look "
                             "no further");}
    

    {   SerialBlock &b          = materializeBlock(NULL, 215, 2, gun1, line, SHAPE_OCTAGON,
                                                   Block::CANNON|Block::TURRET);
        b.name                  = _("Heavy Turret");
        b.scale            = 3;
        b.turretSpeed           = 2.f;
        b.cannon->color         = projectile;
        b.cannon->range         = 1200;
        b.cannon->damage        = 600;
        b.cannon->explosive     = EExplosive::ENABLED;
        b.cannon->explodeRadius = 50;
        b.cannon->roundsPerSec  = 1;
        b.cannon->muzzleVel     = 600; }

    {   SerialBlock &b          = materializeBlock(NULL, 261, 2, fillgen, line, SHAPE_OCTAGON,
                                                   Block::GENERATOR);
        b.name                  = _("Power Distributor");
        b.scale            = 1;
        b.generatorCapacityPerSec = 100; }

    {   SerialBlock &b           = materializeBlock(NULL, 262, 2, gun, line, SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                   = _("Station Battery");
        b.fillColor1             = gun01;
        b.scale             = 3;
        b.cannon->color          = projectile;
        b.cannon->range          = 1300;
        b.cannon->damage         = 75;
        b.cannon->explosive      = EExplosive::ENABLED;
        b.cannon->explodeRadius = 20;
        b.cannon->roundsPerSec   = 1;
        b.cannon->muzzleVel      = 1000;
        b.cannon->burstyness     = 0;}

    {   SerialBlock &b           = materializeBlock(NULL, 263, 2, laser, line,
                                                    SHAPE_SQUARE, Block::LASER|Block::TURRET|Block::AUTOFIRE);
        b.name                   = _("Station Defense Laser");
        b.fillColor1             = laser1;
        b.turretSpeed            = 20.f;
        b.laser->color             = 0x80000000|projectile;
        b.laser->range             = 700;
        b.laser->damage            = 100;
        b.laser->width             = 3;
        b.laser->pulsesPerSec      = 2;
        b.laser->pulseAvailability = 0.8f; }

    for (uint i=2; i<5; i++)
    {
        {   SerialBlock &b = materializeBlock(NULL, 264 + i, 2, fillbase, line,
                                              SHAPE_SQUARE, 0);
            b.fillColor1 = fillbase1;
            b.scale = i;
            b.name  = _("Hull");}

        {   SerialBlock &b = materializeBlock(NULL, 268 + i, 2, fillbase, line,
                                              SHAPE_RIGHT_TRI, 0);
            b.fillColor1 = fillbase1;
            b.scale = i;
            b.name  = _("Hull");}
    }

    {   SerialBlock &b  = materializeBlock(NULL, 273, 2, fillthrust, linethrust,
                                           SHAPE_THRUSTER, Block::THRUSTER);
        b.name          = _("Thruster");
        b.thrusterForce = 500000;
        b.scale    = 4;
        b.durability        = 10.f;
        b.blurb         = _("Provides a large amount of thrust");}

    {   SerialBlock &b  = materializeBlock(NULL, 274, 2, fillcommand1, line,
                                           SHAPE_SQUARE, Block::FACTORY|Block::TRACTOR);
        b.name         = _("Factory");
        b.tractorRange = 600.f;
        b.fillColor1   = fillbase;
        b.scale   = 4; }

    {   SerialBlock &b  = materializeBlock(NULL, 275, 2, fillcommand1, line,
                                           SHAPE_OCTAGON, Block::TRACTOR);
        b.name         = _("Tractor");
        b.tractorRange = 600.f;
        b.fillColor1   = fillbase;
        b.scale   = 2; }
}

static void factionThree()
{
    const uint fatfill      = 0x4d1308;
    const uint thinfill     = 0xe1a71c;
    const uint gun          = 0x7a0a28;
    const uint linegun      = 0xe0fa08;
    const uint line         = 0xff0100;
    const uint command      = 0xe06000;
    const uint fillthrust   = 0xa01d10;
    const uint fillmissile  = 0x901e30;
    const uint fillgen      = 0x602d10;
    const uint fillfac      = 0xff8000;
    const uint hfill        = 0xe0da40;
    const uint thrustColor  = 0xffffff;
    const uint thrustColor1 = 0xff8000;

    const uint projectile  = 0xff2100;
    const uint hprojectile = 0xffdf00;
    
    {   SerialBlock &b = materializeBlock(NULL, 300, 3, command, line,
                                          SHAPE_RHOMBUS_72_108, 
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Nexus");
        b.command->disruptTime    = 0.5f;
        b.generatorCapacityPerSec = 50; }

    {   SerialBlock &b = materializeBlock(NULL, 301, 3, thinfill, line, SHAPE_RHOMBUS_36_144, 0);
        b.name        = _("Penrose Zero");
        b.growRate        = 40.f;
        b.density         = 0.7f * kComponentDensity;
        b.durability      = 0.8f; }

    {   SerialBlock &b = materializeBlock(NULL, 302, 3, fatfill, line, SHAPE_RHOMBUS_72_108, 0);
        b.name        = _("Penrose One");
        b.growRate        = 40.f;
        b.density         = 0.7f * kComponentDensity;
        b.durability      = 0.8f; }

    {   SerialBlock &b = materializeBlock(NULL, 303, 3, gun, linegun, SHAPE_RHOMBUS_72_108,
                                          Block::TURRET|Block::CANNON);
        b.name                 = _("Blaster");
        b.scale           = 1;
        b.cannon->color        = projectile;
        b.cannon->range        = 600;
        b.cannon->damage       = 20;
        b.cannon->roundsPerSec = 5;
        b.cannon->muzzleVel    = 1200;
        b.durability            = 0.4f; }

    {   SerialBlock &b = materializeBlock(NULL, 315, 3, fillthrust, line,
                                          SHAPE_DISH_THRUSTER, Block::THRUSTER);
        b.name           = _("Thruster");
        b.blurb          = _("Mini size");
        b.thrusterForce  = 10000;
        b.thrusterColor  = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.durability      = 1.f; }

    {   SerialBlock &b = materializeBlock(NULL, 304, 3, fillthrust, line,
                                          SHAPE_THRUSTER_PENT, Block::THRUSTER);
        b.name          = _("Thruster");
        b.blurb          = _("Small size");
        b.thrusterForce = 30000;
        b.thrusterColor  = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.durability        = 0.5f; }
    
    {   SerialBlock &b = materializeBlock(NULL, 305, 3, fillthrust, line,
                                          SHAPE_THRUSTER_PENT, Block::THRUSTER);
        b.name          = _("Thruster");
        b.blurb          = _("Medium size");
        b.thrusterForce = 70000;
        b.thrusterColor  = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.scale    = 2;}
    
    {   SerialBlock &b = materializeBlock(NULL, 306, 3, fillthrust, line,
                                          SHAPE_THRUSTER_PENT, Block::THRUSTER);
        b.name          = _("Thruster");
        b.blurb          = _("Large size");
        b.thrusterForce = 150000;
        b.thrusterColor  = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.scale    = 3; }

    {   SerialBlock &b = materializeBlock(NULL, 307, 3, fatfill, line, SHAPE_ADAPTER, 0);
        b.name        = _("Adapter");}
    
    // ---weapon shapes:---
    // RECT_CANNON
    // CANNON
    
    // ---weapon booster shapes:---
    // TRI_CANNON_BOOST
    // RECT_CANNON_BOOST
    // DISH_WEAPON

    {   SerialBlock &b           = materializeBlock(NULL, 309, 3, lightred, lightorange,
                                                    SHAPE_PENTAGON, Block::TURRET|Block::CANNON);;
        b.name                   = _("Phaser");
        b.durability             = 1.5f;
        b.cannon->color          = projectile;
        b.cannon->range          = 1500;
        b.cannon->damage         = 45;
        b.cannon->explosive      = EExplosive::ENABLED;
        b.cannon->explodeRadius  = 15.f;
        b.cannon->roundsPerSec   = 1.f;
        b.cannon->muzzleVel      = 1500;
        b.cannon->spread         = 0.02;
        b.cannon->burstyness     = 0.8;
        b.cannon->roundsPerBurst = 2;}
    
    {   SerialBlock &b           = materializeBlock(NULL, 310, 3, lightred, lightorange,
                                                    SHAPE_PENTAGON, Block::TURRET|Block::CANNON);
        b.name                   = _("Tempest");
        b.scale             = 2;
        b.durability             = 1.5f;
        b.cannon->color          = projectile;
        b.cannon->range          = 1300;
        b.cannon->damage         = 20;
        b.cannon->roundsPerSec   = 40;
        b.cannon->muzzleVel      = 1000;
        b.cannon->spread         = 0.01;
        b.cannon->burstyness     = 0.2;
        b.cannon->roundsPerBurst = 4;}
    
    {   SerialBlock &b = materializeBlock(NULL, 311, 3, fillgen, line,
                                          SHAPE_RHOMBUS_72_108, Block::GENERATOR);
        b.name                    = _("Generator");
        b.durability             = 1.5f;
        b.generatorCapacityPerSec = 100;
        b.capacity                = 400; }

    {   SerialBlock &b = materializeBlock(NULL, 312, 3, darkred, lightorange,
                                          SHAPE_PENTAGON, Block::LASER|Block::TURRET);
        b.name                    = _("Laser");
        b.durability               = 1.5f;
        b.laser->damage             = 250;
        b.laser->range              = 600;
        b.laser->width              = 3;
        b.laser->color              = ALPHA(0.63f)|projectile; }
    
    {   SerialBlock &b = materializeBlock(NULL, 313, 3, fillmissile, line,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                           = _("Drone Launcher");
        b.launcherPower                  = 200;
        b.launcherOutSpeed               = 50.f;
        b.replicateTime                  = 1.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::LASER|Block::FREERES|Block::GENERATOR);
        b.replicateBlock->laser->color            = ALPHAF(0.5)|projectile;
        b.replicateBlock->laser->damage           = 100;
        b.replicateBlock->laser->range            = 400;
        b.replicateBlock->laser->width            = 2;
        b.replicateBlock->shape              = SHAPE_MISSILE;
        b.replicateBlock->thrusterForce           = 4000.f;
        b.replicateBlock->torquerTorque           = 5000;
        b.replicateBlock->thrusterColor           = thrustColor;
        b.replicateBlock->thrusterColor1          = thrustColor1;
        b.replicateBlock->lifetime                = 10;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->fillColor               = fillmissile;
        b.replicateBlock->lineColor               = line; }
    
    {   SerialBlock &b = materializeBlock(NULL, 314, 3, gun, linegun,
                                          SHAPE_PENTAGON, Block::LASER|Block::TURRET|Block::AUTOFIRE);
        b.name          = _("Anti-Missile Laser");
        b.turretSpeed   = 20.f;
        b.laser->damage = 50;
        b.laser->range  = 400;
        b.laser->width  = 2;
        b.laser->color  = ALPHA(0.63f)|projectile; }

    {   SerialBlock &b = materializeBlock(NULL, 316, 3, fillfac, line, SHAPE_RHOMBUS_72_108,
                                          Block::FACTORY|Block::TRACTOR);
        b.name         = _("Factory");
        b.tractorRange = 1000.f;
        b.scale        = 2; }

    {   SerialBlock &b = materializeBlock(NULL, 317, 3, thinfill, line, SHAPE_RHOMBUS_36_144, 0);
        b.name         = _("Storage Pod");
        b.fillColor1   = fatfill;
        b.scale        = 2;
        b.capacity     = 200;
        b.growRate     = 20.f;
        b.density      = 0.7f * kComponentDensity;
        b.durability   = 0.5f; }

    {   SerialBlock &b = materializeBlock(NULL, 318, 3, darkred, lightorange,
                                          SHAPE_PENTAGON, Block::LASER|Block::TURRET);
        b.fillColor1 = hfill;
        b.name          = _("Nanoassembler Beam");
        b.scale         = 2;
        b.durability    = 2.f;
        b.turretSpeed   = 2.f;
        b.laser->damage = -300;
        b.laser->range  = 900;
        b.laser->width  = 6;
        b.laser->decay  = 0.5f;
        b.laser->color  = ALPHA(0.25f)|hprojectile; }

    {   SerialBlock &b = materializeBlock(NULL, 319, 3, hfill, line,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                           = _("Nanoassembler Drone Launcher");
        b.launcherPower                  = 200;
        b.launcherOutSpeed               = 50.f;
        b.replicateTime                  = 2.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::LASER|Block::FREERES|Block::GENERATOR);
        b.replicateBlock->laser->color            = ALPHAF(0.25)|hprojectile;
        b.replicateBlock->laser->damage           = -100;
        b.replicateBlock->laser->range            = 400;
        b.replicateBlock->laser->width            = 4;
        b.replicateBlock->laser->immobilizeForce  = 3000.f;
        b.replicateBlock->shape                   = SHAPE_MISSILE_SHORT;
        b.replicateBlock->scale                   = 1;
        b.replicateBlock->thrusterForce           = 3000.f;
        b.replicateBlock->torquerTorque           = 5000;
        b.replicateBlock->thrusterColor           = thrustColor;
        b.replicateBlock->thrusterColor1          = thrustColor1;
        b.replicateBlock->lifetime                = 10;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->fillColor               = fillfac;
        b.replicateBlock->lineColor               = line; }
}

static void factionFour()
{
    const uint fill       = 0x30203b;
    const uint fill1      = 0x10104b;
    const uint wepfill    = 0x40206b;
    const uint wepfill1   = 0x00204b;
    const uint dronefill  = 0x20201b;
    const uint accent     = 0x800000;
    const uint accentHi   = 0xa03000;
    const uint accent1    = 0x074480;
    const uint accent2    = 0x384098;
    const uint contrast   = 0x071965;
    const uint contrast1  = 0xb00000;
    const uint shield     = 0x556699;
    const uint shield1    = 0x558888;
    const uint contrast2  = 0x69e81d;
    const uint projectile = 0x0769f5;
    
    {   SerialBlock &b = materializeBlock(NULL, 400, 4, contrast, wepfill,
                                          SHAPE_COMMAND, Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("CommandControl");
        b.fillColor1 = accent2;
        b.generatorCapacityPerSec = 40; }

    {   SerialBlock &b = materializeBlock(NULL, 401, 4, accent2, wepfill, SHAPE_SQUARE, Block::GENERATOR);
        b.name                    = _("Generator");
        b.fillColor1 = accentHi;
        b.generatorCapacityPerSec = 100; }

    {   SerialBlock &b = materializeBlock(NULL, 402, 4, fill, accent2, SHAPE_RIGHT_TRI, 0);
        b.fillColor1   = fill1;
        b.name         = _("Strut"); }

    SerialBlock &thruster = materializeBlock(NULL, 403, 4, accent, contrast1,
                                             SHAPE_DISH_THRUSTER, Block::THRUSTER);
    thruster.fillColor1 = accentHi;
    thruster.thrusterBoost = 2.5f;
    {   SerialBlock &b  = thruster;
        b.name          = _("Thruster");
        b.durability    = 3.f;
        b.thrusterColor = accent;
        b.thrusterForce = 10000; }

    {   SerialBlock &b = materializeBlock(&thruster, 404, 4);
        b.name          = _("Thruster");
        b.durability    = 2.f;
        b.scale    = 2;
        b.thrusterForce = 30000; }
        
    {   SerialBlock &b = materializeBlock(&thruster, 405, 4);
        b.name          = _("Thruster");
        b.durability    = 2.f;
        b.scale    = 3;
        b.thrusterForce = 70000; }

    {   SerialBlock &b = materializeBlock(NULL, 406, 4, wepfill, contrast,
                                          SHAPE_RECT_CANNON, Block::CANNON);
        b.name                 = _("CannonBase");
        b.blurb               = _("Modular plasma cannon");
        b.fillColor1       =    wepfill1;
        b.cannon->color        = projectile;
        b.cannon->roundsPerSec = 2;
        b.cannon->range        = 400;
        b.cannon->damage       = 30;
        b.cannon->muzzleVel    = 600;
        b.cannon->spread       = 0.0;  }

    {   SerialBlock &b = materializeBlock(NULL, 407, 4, accent1, contrast,
                                          SHAPE_DISH_WEAPON, Block::CANNON_BOOST);
        b.name                  = _("DamageAmplifier");
        b.blurb                 = _("Increases damage of attached weapon");
        b.fillColor1         =  wepfill1;
        b.boost->damage.y = 30;
        b.boost->power.x = 1.02; }

    {   SerialBlock &b = materializeBlock(NULL, 408, 4, accent1, contrast,
                                          SHAPE_DISH_WEAPON, Block::CANNON_BOOST);
        b.name                  = _("DamageAmplifier");
        b.blurb                 = _("Significantly increases damage of attached weapon");
        b.fillColor1            = wepfill1;
        b.scale            = 2;
        b.boost->damage.y = 60;
        b.boost->power.x = 1.04; }

    {   SerialBlock &b = materializeBlock(NULL, 409, 4, accent1, contrast,
                                          SHAPE_RECT_CANNON_BOOST, Block::CANNON_BOOST);
        b.name                     = _("RangeAmplifier");
        b.blurb                 = _("Increases range and projectile speed of attached weapon");
        b.fillColor1               = wepfill1;
        b.boost->range.y     = 400;
        b.boost->muzzleVel.y = 100; 
        b.boost->power.y     = 10; }

    {   SerialBlock &b = materializeBlock(NULL, 410, 4, accent1, contrast,
                                          SHAPE_ISOTRI_25_WEAPON, Block::CANNON_BOOST);
        b.name                        = _("RateAmplifier");
        b.blurb                 = _("Increases rate of fire of attached weapon");
        b.fillColor1                  = wepfill1;
        b.boost->roundsPerSec.x = 2.f;
        b.boost->power.x        = 1.2f; }

    {   SerialBlock &b = materializeBlock(NULL, 419, 4, accent1, contrast,
                                          SHAPE_DISH_WEAPON, Block::CANNON_BOOST);
        b.name                  = _("VelocityAmplifier");
        b.blurb                 = _("Significantly increases projectile speed of attached weapon");
        b.fillColor1            = wepfill1;
        b.scale            = 3;
        b.boost->muzzleVel.y = 200;
        b.boost->roundsPerSec.x = 1.5;
        b.boost->power.x = 1.1; }

    {   SerialBlock &b = materializeBlock(NULL, 411, 4, shield, contrast2,
                                          SHAPE_SQUARE, Block::SHIELD);
        b.name           = _("Protector");
        b.fillColor1     = shield1;
        b.shield->radius   = 3 * kComponentWidth;
        b.shield->strength = 500;
        b.shield->regen    = 300; }

    {   SerialBlock &b = materializeBlock(NULL, 412, 4, dronefill, contrast2,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                           = _("Drone Launcher");
        b.fillColor1 = wepfill1;
        b.launcherPower                  = 200;
        b.replicateTime                  = 1.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::CANNON|Block::GENERATOR|Block::FREERES);
        b.replicateBlock->cannon->color          = projectile;
        b.replicateBlock->cannon->range          = 700;
        b.replicateBlock->cannon->damage         = 15;
        b.replicateBlock->cannon->roundsPerSec   = 5;
        b.replicateBlock->cannon->muzzleVel      = 800;
        b.replicateBlock->cannon->spread         = 0.05;
        b.replicateBlock->cannon->burstyness     = 0.3;
        b.replicateBlock->cannon->roundsPerBurst = 4; 
        b.replicateBlock->cannon->power          = 8;
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterForce = 4000.f;
        b.replicateBlock->torquerTorque = 4000;
        b.replicateBlock->lifetime      = 10;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->fillColor     = contrast2;
        b.replicateBlock->lineColor     = projectile;
        b.replicateBlock->thrusterColor = accent; }

    {   SerialBlock &b = materializeBlock(NULL, 413, 4, wepfill, contrast, SHAPE_SQUARE,
                                          Block::LASER|Block::TURRET|Block::AUTOFIRE);
        b.name          = _("Defense Laser");
        b.fillColor1    = accent2;
        b.turretSpeed   = 20.f;
        b.laser->color  = 0x80000000|projectile;
        b.laser->range  = 500;
        b.laser->damage = 70;
        b.laser->width  = 3; }

    {   SerialBlock &b = materializeBlock(NULL, 414, 4, wepfill, contrast, SHAPE_SQUARE,
                                          Block::LASER|Block::TURRET|Block::AUTOFIRE);
        b.name          = _("Defense Laser");
        b.scale    = 2;
        b.fillColor1    = accent2;
        b.turretSpeed   = 3.f;
        b.laser->color  = 0x80000000|projectile;
        b.laser->range  = 800;
        b.laser->damage = 250;
        b.laser->width  = 5; }

    {   SerialBlock &b = materializeBlock(NULL, 415, 4, shield, contrast2,
                                          SHAPE_SQUARE, Block::SHIELD);
        b.name           = _("Protector");
        b.fillColor1     = shield1;
        b.scale     = 2;
        b.shield->radius   = 10 * kComponentWidth;
        b.shield->strength = 2500;
        b.shield->regen    = 500; }

    {   SerialBlock &b = materializeBlock(NULL, 416, 4, shield, fill1,
                                          SHAPE_SQUARE, Block::PHOTOSYNTH);
        b.name             = _("Solar Array");
        b.fillColor1       = contrast;
        b.scale       = 2;
        b.photosynthPerSec = 0.5f; }

    {   SerialBlock &b = materializeBlock(NULL, 417, 4, shield, fill1,
                                          SHAPE_SQUARE, Block::PHOTOSYNTH);
        b.name             = _("Solar Array");
        b.fillColor1       = contrast;
        b.scale       = 3;
        b.photosynthPerSec = 0.75f; }
    
    {   SerialBlock &b = materializeBlock(NULL, 418, 4, wepfill1, fill1,
                                          SHAPE_SQUARE, Block::FACTORY|Block::TRACTOR);
        b.name             = _("Factory");
        b.tractorRange     = 600.f;
        b.fillColor1       = accent;
        b.scale       = 3;}

    {   SerialBlock &b = materializeBlock(NULL, 420, 4, accent1, fill1,
                                          SHAPE_RIGHT_TRI, 0);
        b.name             = _("RBank");
        b.capacity         = 1000.f;
        b.fillColor1       = contrast;
        b.scale       = 4;}

    {   SerialBlock &b = materializeBlock(NULL, 421, 4, dronefill, contrast2,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.name                           = _("Rocket Drone Launcher");
        b.fillColor1 = wepfill1;
        b.launcherPower                  = 400;
        b.replicateTime                  = 1.5f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::LAUNCHER|Block::GENERATOR|Block::FREERES);
        b.replicateBlock->shape    = SHAPE_MISSILE_LAUNCHER;
        b.replicateBlock->scale    = 2;
        b.replicateBlock->thrusterForce = 4000.f;
        b.replicateBlock->torquerTorque = 4000;
        b.replicateBlock->lifetime      = 20;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->fillColor     = contrast2;
        b.replicateBlock->lineColor     = projectile;
        b.replicateBlock->replicateTime = 0.5f;
        b.replicateBlock->launcherSpeed = 500; 
        SerialBlock &rocket = *b.replicateBlock->replicateBlock;
        rocket.setFeatures(Block::THRUSTER|Block::EXPLODE);
        rocket.explodeDamage = 25;
        rocket.explodeRadius = 10;
        rocket.lifetime      = 2.5f;
        rocket.thrusterForce = 3000.f;
        rocket.shape      = SHAPE_MISSILE;
        rocket.fillColor = contrast2;
        rocket.lineColor = projectile;
        rocket.thrusterColor = accent; }

    {   SerialBlock &b = materializeBlock(NULL, 426, 4, fill, accent2, SHAPE_RECT, 0);
        b.fillColor1 = fill1;
        b.scale = 2;
        b.name       = _("Strut"); }

    const uint shapes[] = { SHAPE_RECT_LONG, SHAPE_RIGHT_TRI };
    const uint maxsize = 4;

    for (uint i=0; i<arraySize(shapes); i++) {
        for (uint j=1; j<maxsize; j++) {
            SerialBlock &b = materializeBlock(NULL, 427 + i * maxsize + j, 4, fill, accent2,
                                              (BlockShape) shapes[i], 0);
            b.fillColor1  = fill1;
            b.scale = j;
            b.name       = _("Strut");
        }
    }
}

struct BlockColors { uint fill, fill1, line; };

static void factionFive()
{
    // plant faction
    const uint basefill    = 0x004800;
    const uint basefill1   = 0x008040;
    const uint baseline    = 0x30a000;
    const uint seedfill    = 0x507000;
    const uint seedfill1   = 0x20a060;
    const uint cropfill    = 0xe09010;
    const uint cropfill1   = 0xc09030;
    const uint cropline    = 0xf0d070;

    static DEFINE_CVAR(float, kSeedGrowRate, 0.1f);

    const BlockColors colors[] = {
        { basefill, basefill1, baseline },
        { cropfill, cropfill1, cropline },
        { 0x9090a0, 0x808090, 0xb0b0c0 },
        { 0x6010C0, 0xb02080, 0xF0404E },
        { 0x006060, 0x10a080, 0x3090b0 },
    };

    const BlockColors scolors[] = {
        { 0x007050, 0x109050, 0x1080ff },
        { 0x007090, 0x0060b0, 0xeb008f },
    };

    // when planted, plants last for seedLifetime
    // when seeds are free floating, they last for launchLifetime

    for (uint i=0; i<4; i++)
    {
        SerialBlock& b = materializeBlock(NULL, 500 + i, 5, seedfill, baseline,
                                          (BlockShape) (SHAPE_SEED_1 + i),
                                          Block::SEED|Block::PHOTOSYNTH|Block::THRUSTER);
        b.name             = _("Seed");
        b.fillColor1       = seedfill1;
        b.thrusterForce    = 2000.f;
        b.seedLifetime     = 180.f;
        b.launchLifetime   = 10.f;
        b.capacity         = 5.f;
        b.launchResources  = 1.25f;
        b.launchSpeed      = 200.f;
        b.photosynthPerSec = 0.2;
        b.growRate         = kSeedGrowRate;
        b.durability       = 0.5f;
        b.thrusterColor    = 0xffffff;
        b.thrusterColor1   = basefill;
    }

    SerialBlock& cropSeed = materializeBlock(NULL, 504, 5, cropfill, cropline, SHAPE_SEED_3,
                                             Block::SEED|Block::PHOTOSYNTH);
    {   SerialBlock& b = cropSeed;
        b.name             = _("Crop Seed");
        b.fillColor1       = cropfill1;
        b.seedLifetime     = 60.f;
        b.launchLifetime   = 10.f;
        b.capacity         = 20.f;
        b.launchResources  = 5.f;
        b.launchSpeed      = 50.f;
        b.photosynthPerSec = 1.f;
        b.growRate         = 1.f;
        b.durability        = 0.25f; }

    { SerialBlock& b = materializeBlock(NULL, 510, 5, basefill, baseline, SHAPE_RECT_LONG_NARROW, 0);
        b.name       = _("Stem");
        b.durability  = 0.25f;
        b.fillColor1 = basefill1;
        b.growRate   = 2.f;
        b.scale = 3; }

    { SerialBlock& b = materializeBlock(NULL, 511, 5, basefill, cropline, SHAPE_RECT_LONG_NARROW, 0);
        b.name       = _("Crop Stem");
        b.durability  = 0.25f;
        b.fillColor1 = cropfill;
        b.growRate   = 2.f;
        b.scale = 3;  }

    uint maxjsize = 3;
    for (uint i=0; i<3; i++)
    {
        for (uint j=1; j<maxjsize; j++)
        {
            { SerialBlock& b = materializeBlock(NULL, 512 + i + maxjsize * (j-1), 5, basefill, baseline,
                                                (BlockShape) (SHAPE_GEM_2 + i), 0);
                b.fillColor1 = basefill1;
                b.name       = _("Joint");
                b.scale = j;
                b.durability  = 0.25f;
                b.growRate   = 2.f;  }
        }
    }

    const BlockShape lshapes[] = {
        SHAPE_TRI, SHAPE_OCTAGON, SHAPE_PENTAGON };
    for (uint i=0; i<arraySize(lshapes); i++) {
        for (uint j=1; j<4; j++) {
            for (uint k=0; k<arraySize(colors); k++)
            {
                uint ident = 520 + i + arraySize(lshapes) * (j-1) + arraySize(lshapes) * arraySize(colors) * k;
                if (ident == 530)
                    ident = 5299;
                SerialBlock& b = materializeBlock(NULL, ident, 5, colors[k].fill, colors[k].line,
                                                  lshapes[i], Block::PHOTOSYNTH); 
                b.name             = _("Leaf");
                b.fillColor1       = colors[k].fill1;
                b.photosynthPerSec = 0.1 * j * j;
                b.scale       = j;
            }
        }
    }

    {   SerialBlock& b = materializeBlock(NULL, 530, 2, cropfill, 0xaef300, SHAPE_RECT_LAUNCHER1,
                                          Block::LAUNCHER);
        b.name = _("Seed Planter");
        b.replicateBlock = cropSeed;
        b.launcherPower  = 10.f;
        b.launcherSpeed  = 500.f;
        b.replicateTime  = 5.f; }


    const BlockShape shapes[] = { 
        SHAPE_PENTAGON, SHAPE_ISOTRI_72, 
        SHAPE_ISOTRI_36, SHAPE_OCTAGON, 
        SHAPE_RECT_LONG_NARROW, SHAPE_GEM_2,
        SHAPE_GEM_3, SHAPE_GEM_4 };
    for (uint i=0; i<arraySize(shapes); i++) {
        for (uint j=1; j<maxjsize; j++) {
            for (uint k=0; k<arraySize(colors); k++)
            {
                SerialBlock& b = materializeBlock(NULL, 5000 + i + arraySize(shapes) * (j-1) + arraySize(shapes) * maxjsize * k,
                                                  5, colors[k].fill, colors[k].line,
                                                  shapes[i], 0);

                b.name = (shapes[i] == SHAPE_RECT_LONG_NARROW) ? _("Stem") : _("Joint");
                b.scale = (shapes[i] == SHAPE_RECT_LONG_NARROW) ? j * 3 : j;
                b.fillColor1 = colors[k].fill1;
                b.durability  = 0.25f;
                b.growRate   = 2.f;
            }
        }
    }

    for (uint j=1; j<maxjsize; j++) {
        for (uint k=0; k<arraySize(colors); k++)
        {
            SerialBlock& b = materializeBlock(NULL, 5150 + (j-1) + maxjsize * k,
                                              5, colors[k].fill, colors[k].line,
                                              SHAPE_ADAPTER, 0);
            b.scale = j;
            b.fillColor1 = colors[k].fill1;
            b.durability  = 0.25f;
            b.growRate   = 2.f;
        }
    }

    for (uint i=0; i<4; i++)
    {
        for (uint j=0; j<arraySize(scolors); j++) {
            SerialBlock& b = materializeBlock(NULL, 5300 + i + 4 * j, 5, 
                                              scolors[j].fill, scolors[j].line,
                                              (BlockShape) (SHAPE_SEED_1 + i),
                                              Block::SEED|Block::PHOTOSYNTH|Block::THRUSTER);
            uint scale = j+2;
            b.name             = _("Seed");
            b.fillColor1       = scolors[j].fill1;
            b.thrusterForce    = 2000.f;
            b.seedLifetime     = 90.f + 30.f * j;
            b.launchLifetime   = 10.f;
            b.capacity         = 5.f * scale * scale;
            b.launchResources  = 1.25f * scale * scale;
            b.launchSpeed      = 200.f;
            b.photosynthPerSec = 0.1 * scale * scale;
            b.growRate         = kSeedGrowRate;
            b.durability        = 0.25f; 
            b.thrusterColor    = 0xffffff;
            b.thrusterColor1   = scolors[j].fill;
        }
    }
}

static void factionSix()
{
    const uint fill       = 0xe0e0e0;
    const uint fill1      = 0xffffff;
    const uint line       = 0x606060;
    const uint tfill      = 0xe0e0e0;
    const uint tfill1     = 0xb0b0b0;
    const uint afill      = 0x81a0b9;
    const uint afill1     = 0x8b96ac;
    const uint dfill      = 0x8b96ac;
    const uint dfill1     = 0x6699CC;
    const uint cfill      = 0x66bC99;
    const uint cfill1     = 0x66aCa9;
    const uint projectile = 0x00efff;
    const uint cprojectile = 0x00ffbf;
    const uint aline      = 0x606060;
    const uint thrust     = 0x20ffffff;

    { SerialBlock& b = materializeBlock(NULL, 6000, 6, afill, aline, SHAPE_COMMAND,
                                        Block::COMMAND|Block::TORQUER|Block::PHOTOSYNTH|Block::GENERATOR|Block::ASSEMBLER);
        b.name             = _("Command");
        b.photosynthPerSec = 1.f;
        b.generatorCapacityPerSec = 50.f;
        b.fillColor1       = fill1;
        b.torquerTorque    = 40000.f;
        b.capacity         = 500.f; }

    { SerialBlock& b = materializeBlock(NULL, 6001, 6, afill, aline, SHAPE_COMMAND,
                                        Block::COMMAND);
        b.name             = _("Subcommand");
        b.fillColor1       = fill1;
        b.capacity         = 500.f; }
    
    { SerialBlock& b = materializeBlock(NULL, 6002, 6, fill, line, SHAPE_RECT_ROOT,
                                        Block::ROOT);
        b.name             = _("Root");
        b.fillColor1       = fill1; }

    const BlockShape shapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI, SHAPE_RECT_LONG, 
                                  SHAPE_OCTAGON, SHAPE_RECT_V1, SHAPE_THRUSTER_RECT,
                                  SHAPE_SQUARE_HALF, SHAPE_ADAPTER };
    const uint maxsize = 4;

    for (uint i=1; i<maxsize; i++)
    {
        for (uint j=0; j<arraySize(shapes); j++)
        {
            if (!hasShapeSpec(shapes[j], i))
                continue;
            SerialBlock &b = materializeBlock(NULL, 6010 + maxsize * j + (i-1), 6,
                                              fill, line, shapes[j], Block::INTLINES);
            b.scale = i;
            b.fillColor1 = fill1;
            
            if (shapes[j] == SHAPE_THRUSTER_RECT)
            {
                b.features      |= Block::THRUSTER;
                b.thrusterForce  = 20000 * i * i;
                b.fillColor      = tfill;
                b.fillColor1     = tfill1;
                b.thrusterColor  = thrust;
            }

            if (shapes[j] == SHAPE_OCTAGON && i == 3) {
                b.scale = 4;
                b.shape = SHAPE_SQUARE;
            }

            if (shapes[j] == SHAPE_RECT_LONG && i == 1) {
                b.shape = SHAPE_SQUARE;
                b.features = Block::GENERATOR;
                b.fillColor = tfill;
                b.fillColor1 = tfill1;
                b.generatorCapacityPerSec = 40;
            }
        }
    }

    for (uint j=1; j<4; j++) {
        for (uint i=0; i<2; i++)
        {
            SerialBlock &b = materializeBlock(NULL, 60100 + (j-1) * 2 + i, 6, (i==0) ? afill : dfill, aline,
                                              SHAPE_SQUARE,
                                              Block::LASER|(i==0 ? Block::TURRET : Block::TURRET|Block::AUTOFIRE));
            
            b.name        = lstring(str_format(_("Laser %d:%d"), j, i));
            b.scale  = j;
            b.fillColor1  = (i==0) ? afill1 : dfill1;
            b.turretSpeed = 7.f / j;
            b.laser->color  = 0x80000000|projectile;
            b.laser->range  = 1000;
            b.laser->damage = 75 * j * j;
            b.laser->width  = 3 * j; 
        }

        {
            SerialBlock& b = materializeBlock(NULL, 60110 + (j-1), 6, afill, aline,
                                              SHAPE_SQUARE_LAUNCHER, Block::LAUNCHER);
            b.replicateTime    = 4.f;
            b.fillColor1       = afill1;
            b.launcherOutSpeed = 200;
            b.scale       = j;
            b.replicateTime    = 1.f;
            b.name             = _("Laser Drone Launcher");
            b.replicateBlock->setFeatures(Block::TURRET|Block::AUTOFIRE|Block::LASER|Block::TORQUER|Block::THRUSTER|Block::COMMAND|
                                          Block::FREERES|Block::GENERATOR);
            b.replicateBlock->name          = "Laser Drone";
            b.replicateBlock->shape    = SHAPE_SQUARE_MISSILE;
            b.replicateBlock->scale    = j;
            b.replicateBlock->launchSpeed   = 0.f;
            b.replicateBlock->thrusterForce = 20000.f;
            b.replicateBlock->torquerTorque = 16000;
            b.replicateBlock->thrusterColor = thrust;
            b.replicateBlock->durability     = 0.5f;
            b.replicateBlock->lifetime      = 20;
            b.replicateBlock->laser->color    = ALPHAF(0.5f)|projectile;
            b.replicateBlock->laser->range    = 600;
            b.replicateBlock->laser->damage   = 65 * j * j;
            b.replicateBlock->laser->width    = 2 * j;
            b.replicateBlock->laser->immobilizeForce = 1000.f * j;
            b.replicateBlock->fillColor     = fill;
            b.replicateBlock->fillColor1    = fill1;
            b.replicateBlock->lineColor     = aline;
            b.replicateBlock->generatorCapacityPerSec = 40;
        }
    }

    {   SerialBlock& b = materializeBlock(NULL, 60120, 6, cfill, line, SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.name             = _("Immobilizer");
        b.fillColor1       = cfill1;
        b.laser->color    = ALPHAF(0.5f)|cprojectile;
        b.laser->range    = 1100;
        b.laser->damage   = 0.f;
        b.laser->immobilizeForce = 100000.f;
        b.laser->linearForce = -100000;
        b.laser->width    = 3.f;
    }
}

static void factionSeven()
{
    const uint basefill    = 0x783201;
    const int basefill1    = 0xE0A231;
    const uint baseline    = 0xC57F4C;
    const uint thrustfill  = 0x905000;
    const uint thrustline  = 0xBE883E;
    const uint thrustcolor = 0xDECD78;
    const uint projectile  = 0x90e020;
    const uint wfill       = 0x111E2B;
    const uint wfill1      = 0x212E4B;
    const uint wline       = 0x57616A;

    {   SerialBlock &b = materializeBlock(NULL, 700, 7, basefill, baseline, SHAPE_ISOTRI_72,
                                          Block::COMMAND|Block::GENERATOR|Block::SELFFACTORY|Block::TRACTOR|
                                          Block::ASSEMBLER|Block::TELEPORTER);
        b.name           = _("Sensoria");
        b.fillColor1     = wfill;
        b.generatorCapacityPerSec = 100;
        b.growRate               = 5.f;
        b.scale     = 1; }

    {   SerialBlock &b = materializeBlock(NULL, 701, 7, basefill, baseline, SHAPE_RHOMBUS_36_144, 0);
        b.fillColor1   = basefill1;
        b.name         = _("Chitin"); }

    {   SerialBlock &b = materializeBlock(NULL, 702, 7, basefill, baseline, SHAPE_GEM_3, 0);
        b.fillColor1   = basefill1;
        b.name         = _("Chitin"); }

    {   SerialBlock &b  = materializeBlock(NULL, 703, 7, thrustfill, thrustline, SHAPE_DISH_THRUSTER, Block::THRUSTER);
        b.name          = _("Propulsor");
        b.scale    = 2;
        b.thrusterColor = thrustcolor;
        b.thrusterColor1  = projectile;
        b.thrusterForce = 30000; }

    {   SerialBlock &b           = materializeBlock(NULL, 704, 7, wfill, wline,
                                                    SHAPE_GEM_4, Block::CANNON|Block::TURRET);
        b.name                   = _("Sting");
        b.fillColor1             = wfill1;
        b.growRate               = 2.5f;
        b.cannon->color          = projectile;
        b.cannon->range          = 1200;
        b.cannon->damage         = 4;
        b.cannon->roundsPerSec   = 10;
        b.cannon->muzzleVel      = 1000;
        b.cannon->spread         = 0.02;
        b.cannon->burstyness     = 0.2;
        b.cannon->roundsPerBurst = 5; }
}

static void factionEight()
{
    const uint ofill          = 0x113077;
    const uint ofill1         = 0x205079;
    const uint ofill2         = 0x207089; // missiles / drones
    const uint oline          = 0x3390eb;
    const uint oshot          = 0x00b7e9;
    const uint oshot2         = 0x80b7e9;
    const uint dfill          = 0x414077;
    const uint dfill1         = 0x417078;
    const uint fill           = 0xaaaaaa;
    const uint fill1          = 0x777777;
    const uint baseline       = 0xaab0c0;
    const uint tank           = 0x476676;
    const uint tank1          = 0x324242;
    const uint thrusterColor  = 0x444444;
    const uint thrusterColor1 = 0x445480;
    const uint thrusterLine   = 0x4270b0;
    const uint harvestfill    = 0x404040;
    const uint thrustColor1   = 0x4060a0;
    const uint thrustColor    = 0x808080;
    
    
    {   SerialBlock &b = materializeBlock(NULL, 800, 8, fill, oline,
                                          SHAPE_COMMAND, 
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER|
                                          Block::THRUSTER|Block::TORQUER);
        b.fillColor1              = ofill1;
        b.name                    = _("Command Module");
        b.thrusterForce           = 4000.f;
        b.torquerTorque           = 4000.f;
        b.durability              = 2.f;
        b.generatorCapacityPerSec = 300;
        b.tractorRange            = 500; }

    {   SerialBlock &b = materializeBlock(NULL, 833, 8, fill, oline, SHAPE_COMMAND, 
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER|Block::DEACTIVATES|Block::FACTORY);
        b.name                    = _("Station Command");
        b.fillColor1              = ofill1;
        b.scale              = 3;
        b.durability              = 3.f;
        b.density                 = 2000.f * kComponentDensity;
        b.generatorCapacityPerSec = 5000;
        b.tractorRange            = 1400; }

    {   SerialBlock &b = materializeBlock(NULL, 801, 8, thrusterColor, baseline, SHAPE_SQUARE, Block::GENERATOR);
        b.fillColor1              = thrusterColor1;
        b.name                    = _("Generator");
        b.blurb                   = _("Power your weapons");
        b.density                 = 2.f * kComponentDensity;
        b.generatorCapacityPerSec = 75; }

    {   SerialBlock &b = materializeBlock(NULL, 834, 8, thrusterColor, baseline, SHAPE_SQUARE, Block::GENERATOR);
        b.fillColor1              = thrusterColor1;
        b.name                    = _("Generator");
        b.blurb                   = _("Power your weapons (more)");
        b.scale              = 2;
        b.density                 = 2.f * kComponentDensity;
        b.generatorCapacityPerSec = 450; }

    {   SerialBlock &b = materializeBlock(NULL, 838, 8, fill1, baseline, SHAPE_SQUARE, 0);
        b.name         = _("Transformer");
        b.scale   = 4;
        b.fillColor1   = ofill;
    }

    {   SerialBlock &b = materializeBlock(NULL, 813, 8, harvestfill, fill, SHAPE_SQUARE, 
                                          Block::TRACTOR);
        b.tractorRange            = 1000;
        b.scale              = 2;
        b.name                    = _("Harvester");
        b.blurb                   = _("Increase resource pickup range");}

    {   SerialBlock &b = materializeBlock(NULL, 814, 8, ofill, oline, SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.name                    = _("Laser");
        b.blurb                   = _("Inexpensive short range laser, deadly in quantity");
        b.fillColor1              = ofill1;
        b.scale              = 2;
        b.laser->damage             = 100;
        b.laser->range              = 400;
        b.laser->width              = 4;
        b.laser->color              = ALPHAF(0.5f)|oshot; }

    {   SerialBlock &b = materializeBlock(NULL, 815, 8, ofill2, oline, SHAPE_RIGHT_TRI, Block::LASER|Block::TURRET);
        b.name                    = _("Manipulator");
        b.blurb                   = _("Grabber beam");
        b.fillColor1              = dfill;
        b.scale                   = 2;
        b.laser->damage           = 0;
        b.laser->immobilizeForce  = 50000;
        b.laser->range            = 700;
        b.laser->width            = 6;
        b.laser->color            = ALPHAF(0.5f)|oshot2; }

    const BlockShape shapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI2L, SHAPE_RIGHT_TRI2R, SHAPE_ADAPTER,
                                  SHAPE_OCTAGON };
    const float kHullDurability = 2.5f;
    const float kHullGrowth = 30.f;

    for (uint i=1; i<4; i++)
    {
        const int idoff = (i == 1) ? 0 : (40 + 10 * (i-1));

        
        {   SerialBlock &b = materializeBlock(NULL, 802 + idoff, 8, fill, baseline, SHAPE_RIGHT_TRI, 0);
            b.fillColor1   = fill1;
            b.name         = _("Hull");
            b.blurb        = _("Armored building block");
            b.durability   = kHullDurability;
            b.growRate     = kHullGrowth;
            b.scale   = i; }

        for (uint j=0; j<arraySize(shapes); j++)
        {
            SerialBlock &b = materializeBlock(NULL, 826 + idoff + j, 8, fill, baseline, shapes[j], 0);
            b.fillColor1  = fill1;
            b.name        = _("Hull");
            b.blurb        = _("Armored building block");
            b.scale  = i;
            b.growRate    = kHullGrowth;
            b.durability  = kHullDurability;
            if (shapes[j] == SHAPE_OCTAGON) {
                b.fillColor = tank;
                b.fillColor1 = tank1;
                b.name     = _("Container");
                b.blurb        = _("Resource storage");
                b.capacity = round(0.6f * b.getArea(), 10.f);
                b.durability = kHullDurability / 2.f;
                b.density = kComponentDensity / 2.f;
            }
        }

        {   SerialBlock &b          = materializeBlock(NULL, 831 + idoff, 8, ofill, oline, 
                                                       SHAPE_OCTAGON, Block::CANNON|Block::TURRET);
            b.name                   = _("Plasma Cannon");
            b.blurb                  = _("Basic turreted damage dealer");
            b.fillColor1             = ofill1;
            b.scale             = i;
            b.cannon->color          = oshot;
            b.cannon->range          = 1000 + 100 * i;
            b.cannon->damage         = 30 * i * i;
            b.cannon->roundsPerSec   = 4;
            b.cannon->burstyness     = 0.f;
            b.cannon->roundsPerBurst = 3;
            b.cannon->muzzleVel      = 1400;
            b.cannon->spread         = 0.02f; }
    }

    SerialBlock &thruster = materializeBlock(NULL, 803, 8, thrusterColor, thrusterLine,
                                             SHAPE_DISH_THRUSTER, Block::THRUSTER);
    {   SerialBlock &b   = thruster;
        b.fillColor1     = thrusterColor1;
        b.name           = _("Thruster");
        b.blurb          = _("Increase mobility - mini size");
        b.durability     = 8.f;
        b.thrusterColor  = thrustColor;
        b.thrusterColor1 = thrustColor1;
        b.thrusterForce  = 10000; }

    {   SerialBlock &b  = materializeBlock(&thruster, 804, 8);
        b.name          = _("Thruster");
        b.blurb         = _("Increase mobility - medium size");
        b.durability    = 4.f;
        b.scale    = 2;
        b.thrusterForce = 25000; }
        
    {   SerialBlock &b  = materializeBlock(&thruster, 805, 8);
        b.name          = _("Thruster");
        b.blurb         = _("Increase mobility - mega size");
        b.durability    = 4.f;
        b.scale    = 3;
        b.thrusterForce = 100000; }

    {   SerialBlock &b  = materializeBlock(&thruster, 806, 8);
        b.name          = _("Thruster");
        b.blurb         = _("Increase mobility - asteroid relocation size");
        b.thrusterBoost = -1.f;
        b.thrusterBoostTime = 2.f;
        b.durability    = 1.f;
        b.shape    = SHAPE_THRUSTER;
        b.scale    = 10;
        b.thrusterForce = 1000000; }

    {   SerialBlock &b = materializeBlock(NULL, 832, 8, ofill, oline,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name                          = _("Missile Launcher");
        b.blurb         = _("Launches homing missiles");
        b.fillColor1                    = ofill1;
        b.launcherPower                 = 20;
        b.launcherOutSpeed              = 50.f; 
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateTime                 = 1.5f;
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterForce = 2600.f;
        b.replicateBlock->torquerTorque = 4000.f;
        b.replicateBlock->durability     = 0.4f;
        b.replicateBlock->lifetime      = 5;
        b.replicateBlock->explodeDamage = 30;
        b.replicateBlock->explodeRadius = 25;
        b.replicateBlock->fillColor     = ofill1;
        b.replicateBlock->fillColor1    = ofill2;
        b.replicateBlock->lineColor     = oline;
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1; }

    {   SerialBlock &b = materializeBlock(NULL, 835, 8, ofill, oline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.name             = _("Drone Launcher");
        b.blurb         = _("Launches intelligent drones armed with plasma cannons");
        b.fillColor1       = ofill1;
        b.launcherOutSpeed = 50.f;
        b.launcherPower    = 150; 
        b.replicateTime    = 3.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::FREERES|
                                      Block::TURRET|Block::CANNON|Block::GENERATOR);
        b.replicateBlock->shape              = SHAPE_DISH_MISSILE;
        b.replicateBlock->scale              = 3;
        b.replicateBlock->thrusterForce           = 5000.f;
        b.replicateBlock->thrusterColor           = thrustColor;
        b.replicateBlock->thrusterColor1          = thrustColor1;
        b.replicateBlock->torquerTorque           = 4000.f;
        b.replicateBlock->durability              = 1.f;
        b.replicateBlock->lifetime                = 30;
        b.replicateBlock->fillColor               = ofill1;
        b.replicateBlock->fillColor1              = ofill2;
        b.replicateBlock->lineColor               = oline;
        b.replicateBlock->thrusterColor           = thrustColor; 
        b.replicateBlock->thrusterColor1          = thrustColor1;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->cannon->color           = oshot;
        b.replicateBlock->cannon->range           = 800;
        b.replicateBlock->cannon->damage          = 15;
        b.replicateBlock->cannon->roundsPerSec    = 4;
        b.replicateBlock->cannon->muzzleVel       = 600;
        b.replicateBlock->cannon->spread          = 0.05;
        b.replicateBlock->cannon->burstyness      = 0.6;
        b.replicateBlock->cannon->roundsPerBurst  = 4; }

    {   SerialBlock &b   = materializeBlock(NULL, 836, 8, ofill, oline, 
                                                   SHAPE_CANNON, Block::LASER);
        b.fillColor1     = ofill1;
        b.name           = _("Proton Beam");
        b.blurb         = _("Long range high damage spinal beam weapon");
        b.durability     = 0.25f;
        b.scale     = 2;
        b.laser->color   = ALPHAF(0.5f)|oshot;
        b.laser->range   = 1800;
        b.laser->damage  = 400;
        b.laser->linearForce = 30000;
        b.laser->width   = 5; }

    {   SerialBlock &b          = materializeBlock(NULL, 837, 8, dfill, oline, 
                                                   SHAPE_RIGHT_TRI, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.fillColor1             = dfill1;
        b.name                   = _("Defense Cannon");
        b.blurb         = _("Short range rapid-fire point defense");
        b.scale             = 1;
        b.cannon->color          = oshot;
        b.cannon->range          = 600;
        b.cannon->damage         = 10;
        b.cannon->roundsPerSec   = 15;
        b.cannon->burstyness     = 0;
        b.cannon->roundsPerBurst = 3;
        b.cannon->muzzleVel      = 700;
        b.cannon->spread         = 0.1; }

    {   SerialBlock &b   = materializeBlock(NULL, 839, 8, ofill, oline, 
                                            SHAPE_CANNON, Block::CANNON);
        b.fillColor1           = ofill1;
        b.name                 = _("Plasma Projector");
        b.blurb         = _("Rapid-fire directional plasma weapon");
        b.scale           = 1;
        b.cannon->color        = oshot;
        b.cannon->range        = 1400;
        b.cannon->damage       = 20;
        b.cannon->roundsPerSec = 10;
        b.cannon->burstyness   = 0;
        b.cannon->muzzleVel    = 1400;
        b.cannon->spread       = 0.05f; }

    {   SerialBlock &b = materializeBlock(NULL, 840, 8, ofill, oline,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.fillColor1     = ofill1;
        b.name           = _("Torpedo Launcher");
        b.blurb = _("Launches high damage unguided torpedoes");
        b.launcherPower  = 20;
        b.launcherSpeed  = 300; 
        b.scale     = 2;
        b.replicateBlock->setFeatures(Block::THRUSTER|Block::EXPLODE);
        b.replicateTime                  = 3;
        b.replicateBlock->shape     = SHAPE_MISSILE;
        b.replicateBlock->scale     = 2;
        b.replicateBlock->thrusterForce  = 6000.f;
        b.replicateBlock->durability      = 1.f;
        b.replicateBlock->lifetime       = 10;
        b.replicateBlock->explodeDamage  = 500;
        b.replicateBlock->explodeRadius  = 40;
        b.replicateBlock->fillColor      = ofill;
        b.replicateBlock->lineColor      = oline;
        b.replicateBlock->thrusterColor  = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1; }

    {   SerialBlock &b   = materializeBlock(NULL, 841, 8, ofill, oline, 
                                            SHAPE_SQUARE, Block::LASER|Block::TURRET);
        b.fillColor1     = ofill1;
        b.name           = _("Proton Sword");
        b.blurb         = _("Extremely high damage but very short range beam weapon");
        b.scale     = 2;
        b.laser->color     = ALPHAF(0.5f)|oshot;
        b.laser->range     = 300;
        b.laser->damage    = 2000;
        b.laser->width     = 4; }

    {   SerialBlock &b           = materializeBlock(NULL, 842, 8, ofill, oline, 
                                                    SHAPE_OCTAGON, Block::CANNON|Block::TURRET);
        b.name                   = _("Railgun");
        b.blurb = _("Very long range turreted sniper weapon");
        b.fillColor1             = ofill1;
        b.scale             = 2;
        b.turretSpeed            = 2.f;
        b.cannon->color          = oshot;
        b.cannon->range          = 2500;
        b.cannon->damage         = 350;
        b.cannon->roundsPerSec   = 0.5;
        b.cannon->burstyness     = 0.f;
        b.cannon->roundsPerBurst = 1;
        b.cannon->muzzleVel      = 1500; }


    {   SerialBlock &b           = materializeBlock(NULL, 843, 8, ofill, oline, 
                                                    SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name                   = _("Autocannon");
        b.blurb = _("Very high rate of fire short range plasma weapon");
        b.fillColor1             = ofill1;
        b.scale             = 1;
        b.cannon->color          = oshot;
        b.cannon->range          = 800;
        b.cannon->damage         = 20;
        b.cannon->roundsPerSec   = 20;
        b.cannon->burstyness     = 0.4f;
        b.cannon->spread         = 0.1f;
        b.cannon->roundsPerBurst = 10;
        b.cannon->muzzleVel      = 1200; }

    {   SerialBlock &b           = materializeBlock(NULL, 844, 8, ofill, oline, 
                                                    SHAPE_OCTAGON, Block::CANNON);
        b.name                   = _("Blast Pulser");
        b.blurb = _("Unidirectional projectile emitter");
        b.fillColor1             = ofill1;
        b.scale             = 2;
        b.cannon->color          = oshot;
        b.cannon->range          = 600;
        b.cannon->damage         = 20;
        b.cannon->roundsPerSec   = 75;
        b.cannon->burstyness     = 1.f;
        b.cannon->spread         = M_PIf;
        b.cannon->roundsPerBurst = 50;
        b.cannon->muzzleVel      = 500; }

    {   SerialBlock &b = materializeBlock(NULL, 845, 8, ofill, oline, SHAPE_SQUARE, Block::LASER|Block::TURRET|Block::CHARGING);
        b.name                    = _("Burst Laser");
        b.blurb = _("Compact charging beam weapon");
        b.fillColor1              = ofill1;
        b.scale              = 1;
        b.turretSpeed             = 4.f;
        b.chargeMaxTime           = 2.f;
        b.laser->pulsesPerSec       = 2.f;
        b.laser->burstyness         = 0.5f;
        b.laser->pulsesPerBurst     = 2;
        b.laser->pulseAvailability  = 0.3f;
        b.laser->damage             = 200;
        b.laser->range              = 1400;
        b.laser->width              = 4;
        b.laser->decay              = 0.5f;
        b.laser->color              = ALPHAF(0.5f)|oshot; }

    for (int i=1; i<6; i++) {
        SerialBlock &b = materializeBlock(NULL, (i == 2) ? 846 : 856+i, 8, fill, baseline, SHAPE_RECT, 0);
        b.fillColor1 = fill1;
        b.durability = kHullDurability;
        b.growRate   = kHullGrowth;
        b.scale = i;
        b.blurb  = string(_("Armor block")) + ", " +
                   ((i == 1) ? "1" CROSS "(1-1/" SQRT2 ") " :
                    (i == 2) ? "1" CROSS ONE_HALF " " :
                    (i == 3) ? "1" CROSS "1/" SQRT2 " " :
                    (i == 4) ? "2" CROSS "(2-2/" SQRT2 ") " :
                    (i == 5) ? "2" CROSS "2/" SQRT2 " " : "") + _("units");
        b.name = _("Hull");
    }

    {   SerialBlock &b = materializeBlock(NULL, 847, 8, tank, baseline, SHAPE_OCTAGON, Block::FACTORY);
        b.name         = _("Factory");
        b.blurb        = _("Create more spaceships");
        b.scale   = 3;
        b.fillColor1   = ofill2; }

    {   SerialBlock &b           = materializeBlock(NULL, 848, 8, ofill, oline, 
                                                    SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::CHARGING);
        b.name                   = _("Plasma Mortar");
        b.blurb = _("Powerful charging splash damage weapon");
        b.fillColor1             = ofill1;
        b.scale             = 2;
        b.chargeMaxTime          = 2.f;
        b.chargeMin              = 0.2f;
        b.cannon->color          = oshot;
        b.cannon->range          = 1200;
        b.cannon->damage         = 100;
        b.cannon->explosive      = EExplosive::FINAL;
        b.cannon->explodeRadius  = 100;
        b.cannon->roundsPerSec   = 20;
        b.cannon->burstyness     = 0.0f;
        b.cannon->spread         = 0.0f;
        b.cannon->roundsPerBurst = 1;
        b.cannon->muzzleVel      = 800; }

    {   SerialBlock &b = materializeBlock(NULL, 849, 8, ofill, oline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.name             = _("Mine Layer");
        b.blurb = _("High damage, large splash radius dumb mines");
        b.fillColor1       = ofill1;
        b.launcherOutSpeed = 40.f;
        b.launcherPower    = 50;
        b.scale       = 2;
        b.replicateTime    = 1.f;
        b.replicateBlock->setFeatures(Block::EXPLODE);
        b.replicateBlock->shape = SHAPE_HEPTAGON;
        b.replicateBlock->scale = 1;
        b.replicateBlock->explodeDamage = 150;
        b.replicateBlock->explodeRadius = 70;
        b.replicateBlock->durability  = 1.f;
        b.replicateBlock->lifetime   = 60;
        b.replicateBlock->fillColor  = ofill1;
        b.replicateBlock->fillColor1 = ofill2;
        b.replicateBlock->lineColor  = oline; }

    
    {   SerialBlock &b = materializeBlock(NULL, 850, 8, ofill, oline,
                                          SHAPE_SQUARE, Block::SHIELD);
        b.name                    = _("Shield Projector");
        b.blurb = _("Absorbs damage - essential for capital ships");
        b.fillColor1              = tank;
        b.scale              = 2;
        b.durability              = 0.25f;
        b.shield->color           = PremultiplyAlphaXXX(tank, 0.2f, 0.1f);
        b.shield->lineColor       = PremultiplyAlphaXXX(tank, 0.4f, 0.05f);
        b.shield->damagedColor    = PremultiplyAlphaXXX(tank, 0.7f, 0.2f);
        // b.shield->color        = PremultiplyAlphaXXX(tank, 0.1f, 0.1f);
        // b.shield->lineColor    = PremultiplyAlphaXXX(tank, 0.5f, 0.05f);
        // b.shield->damagedColor = PremultiplyAlphaXXX(tank, 0.3f, 0.3f);
        b.shield->radius          = 10 * kComponentWidth;
        b.shield->strength        = 1000;
        b.shield->regen           = 200;
        b.shield->delay           = 10.f; }

    {   SerialBlock &b          = materializeBlock(NULL, 851, 8, dfill, oline, 
                                                   SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.fillColor1            = dfill1;
        b.name                  = _("Flak Cannon");
        b.blurb                 = _("Explosive point defense cannon");
        b.scale                  = 1;
        b.turretSpeed           = 12.f;
        b.cannon->color         = oshot;
        b.cannon->range         = 900;
        b.cannon->damage        = 15;
        b.cannon->explosive     = EExplosive::PROXIMITY;
        b.cannon->explodeRadius = 30;
        b.cannon->roundsPerSec  = 3;
        b.cannon->burstyness    = 0;
        b.cannon->muzzleVel     = 1000;
        b.cannon->spread        = 0.0; }

    {   SerialBlock &b          = materializeBlock(NULL, 853, 8, ofill, oline, 
                                                   SHAPE_CANNON, Block::CANNON);
        b.fillColor1             = ofill1;
        b.name                   = _("Antimatter Cannon");
        b.blurb = _("Very long range, high damage spinal mounted death machine");
        b.scale             = 4;
        b.durability             = 0.25f;
        b.cannon->color          = oshot;
        b.cannon->range          = 3000;
        b.cannon->damage         = 500;
        b.cannon->explosive      = EExplosive::FINAL;
        b.cannon->explodeRadius  = 50;
        b.cannon->roundsPerSec   = 0.5;
        b.cannon->burstyness     = 0;
        b.cannon->muzzleVel      = 1800;
        b.cannon->spread         = 0.0; }

    {   SerialBlock &b = materializeBlock(NULL, 854, 8, fill1, baseline, SHAPE_SQUARE, Block::TELESPAWN);
        b.name         = _("Teleporter");
        b.scale   = 4;
        b.fillColor1   = ofill; }

    {   SerialBlock &b = materializeBlock(NULL, 855, 8, ofill, oline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.fillColor1    = ofill1;
        b.name          = _("Rocket Launcher");
        b.blurb         = _("Launches flurries of unguided rockets");
        b.launcherPower = 5;
        b.launcherSpeed = 500; 
        b.scale    = 1;
        b.replicateBlock->setFeatures(Block::THRUSTER|Block::EXPLODE);
        b.replicateTime                  = 0.5;
        b.replicateBlock->shape     = SHAPE_MISSILE;
        b.replicateBlock->scale     = 1;
        b.replicateBlock->thrusterForce  = 4000.f;
        b.replicateBlock->durability     = 0.5f;
        b.replicateBlock->lifetime       = 5;
        b.replicateBlock->explodeDamage  = 40;
        b.replicateBlock->explodeRadius  = 20;
        b.replicateBlock->fillColor      = ofill;
        b.replicateBlock->lineColor      = oline;
        b.replicateBlock->thrusterColor  = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1; }

    {   SerialBlock &b = materializeBlock(NULL, 856, 8, ofill, oline,
                                          SHAPE_SQUARE, Block::SHIELD);
        b.name                 = _("Station Shield Projector");
        b.fillColor1           = tank;
        b.scale           = 3;
        b.density              = 3.f * kComponentDensity;
        b.shield->color        = PremultiplyAlphaXXX(tank, 0.3f, 0.1f);
        b.shield->lineColor    = PremultiplyAlphaXXX(tank, 0.5f, 0.05f);
        b.shield->damagedColor = PremultiplyAlphaXXX(tank, 0.7f, 0.2f);
        b.shield->radius       = 50 * kComponentWidth;
        b.shield->strength     = 5000;
        b.shield->regen        = 500; }

}

static void factionNine()
{
    const uint fill  = 0x5c5c5c;
    const uint fill1 = 0x808080;
    const uint line  = 0xd8d8d8;
        
    {SerialBlock &b = materializeBlock(NULL, 907, 2, darkpurple, line, SHAPE_RECT, Block::ENVIRONMENTAL);
        b.name        = _("Unobtanium Strut");
        b.fillColor1  = medpurple;
        b.density     = 4.f * kComponentDensity;
        b.scale  = 2;
        b.durability   = 20.f;}

    for (int i=0; i<3; i++)
    {
        {SerialBlock &b = materializeBlock(NULL, 900+i, 2, fill, line, SHAPE_SQUARE, Block::ENVIRONMENTAL);
            b.name        = _("Wall");
         b.fillColor1  = fill1;
         b.scale  = i + 1; }

        {SerialBlock &b = materializeBlock(NULL, 904+i, 2, fill, line, SHAPE_RECT_LONG, Block::ENVIRONMENTAL);
            b.name        = _("Extender");
        b.scale  = 2 + i; }

        { SerialBlock &b = materializeBlock(NULL, 908+i, 2, fill, line, SHAPE_ADAPTER, Block::ENVIRONMENTAL);
            b.name        = _("Mount");
        b.scale  = i+1;}
        
        {SerialBlock &b = materializeBlock(NULL, 911+i, 2, fill, line, SHAPE_RIGHT_TRI, Block::ENVIRONMENTAL);
            b.name        = _("Corner");
        b.fillColor1  = fill1;
        b.scale  = i+1;}

        {SerialBlock &b = materializeBlock(NULL, (i == 1) ? 903 : 914+i, 2, fill, line, SHAPE_RECT, Block::ENVIRONMENTAL);
            b.name        = _("Strut");
        b.fillColor1  = fill1;
        b.scale  = 1+i;}
    }
       
    {SerialBlock &b = materializeBlock(NULL, 924, 2, fill, line, SHAPE_HEXAGON, Block::ENVIRONMENTAL);
        b.name        = _("Hex Wall");
        b.fillColor1  = fill1;
        b.scale  = 1;}
    
    {SerialBlock &b = materializeBlock(NULL, 925, 2, fill, line, SHAPE_HEXAGON, Block::ENVIRONMENTAL);
        b.name        = _("Hex Wall");
        b.fillColor1  = fill1;
        b.scale  = 2;}

    {SerialBlock &b = materializeBlock(NULL, 926, 2, medgrey, lightgrey,SHAPE_HEXAGON, Block::ENVIRONMENTAL);
        b.name        = _("Hex Wall");
        b.scale  = 3;}

    const BlockShape shapes[] = { SHAPE_RIGHT_TRI_30L, SHAPE_RIGHT_TRI_30R };
    const uint sizes[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    
    for (uint i=1; i<arraySize(sizes); i++)
    {
        for (uint j=0; j<arraySize(shapes); j++)
        {
            SerialBlock &b = materializeBlock(NULL, 926 + arraySize(sizes) * j + i, 2, fill, line,
                                              shapes[j], Block::ENVIRONMENTAL);
            b.fillColor1  = fill1;
            b.name        = _("Wedge");
            b.scale  = sizes[i];
        }
    }
    for (int i=0; i<4; i++) {
        SerialBlock &b = materializeBlock(NULL, 953+i, 2, fill, line, SHAPE_RECT, Block::ENVIRONMENTAL);
        b.name        = _("Strut");
        b.fillColor1  = fill1;
        b.scale  = 2;
    }

}

static void factionTen()
{
    
}

static void factionEleven()
{
    const uint fill         = 0x043E77;
    const uint fill1        = 0x04167C;
    const uint command      = 0x8080ff;
    const uint command1     = 0x6090ff;
    const uint line         = 0x254DFB;
    const uint weapon       = 0x37006E;
    const uint weapon1      = 0x860285;
    const uint wline        = 0xB00049;
    const uint weapon2      = 0x5580a0;
    const uint lightning    = 0x0677D4;
    const uint lightning1   = 0x12B079;
    const uint projectile   = 0xcf1090;
    const uint thruster     = 0x02149E;
    const uint thruster1    = 0x423687;
    const uint thrustColor  = 0x90CEF0;
    const uint thrustColor1 = 0x90D7F0;
    const uint generator    = 0x02985D;
    const uint generator1   = 0x2F604F;

    const uint shapes[] = { SHAPE_PENTAGON, SHAPE_RHOMBUS_72_108,
                            SHAPE_RHOMBUS_36_144, SHAPE_THRUSTER_PENT,
                            SHAPE_ISOTRI_72, SHAPE_ISOTRI_36 };
    const uint maxsize = 5;

    {   SerialBlock &b = materializeBlock(NULL, 11000, 11, command, line,
                                          SHAPE_ISOTRI_36,
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name       = _("Command");
        b.fillColor1 = command1;
        b.capacity   = 1000;
        b.durability = 4.f; }


    const char* sizes[] = {_("Micro"), _("Medium"), _("Macro"), _("Mighty"), _("Massive")};
    for (int size=1; size<maxsize; size++) {
        for (int sidx = 0; sidx < arraySize(shapes); sidx++) {
            if (!hasShapeSpec(shapes[sidx], size))
                continue;

            const BlockShape shape = (BlockShape) shapes[sidx];
            
            SerialBlock& b = materializeBlock(NULL, 11001 + maxsize * sidx + size, 11,
                                              fill, line, shape, 0);
            b.name       = _("Crystallite");
            b.fillColor1 = fill1;
            b.scale = size;
            b.durability = 2.f;
            b.growRate   = 20.f;

            if (shape == SHAPE_THRUSTER ||
                shape == SHAPE_THRUSTER_PENT ||
                shape == SHAPE_DISH_THRUSTER) 
            {
                b.name = _("Thruster");
                b.features      |= Block::THRUSTER;
                b.thrusterForce  = 22000 * size * max(1, (size-1));
                b.fillColor      = thruster;
                b.fillColor1     = thruster1;
                b.thrusterColor  = thrustColor;
                b.thrusterColor1 = thrustColor1;
                b.name           = _("Booster");
            }

            if (shape == SHAPE_PENTAGON)
            {
                const Feature_t turret = (size == 1) ? Block::AUTOFIRE : Block::ZERO;
                b.setFeatures(b.features|Block::CANNON|Block::TURRET|turret);
                b.fillColor             = weapon;
                b.fillColor1            = weapon1;
                b.lineColor             = wline;
                if (size == 1) {
                    b.name = _("Defense Pulser");
                    b.turretSpeed       = 12.f;
                } else {
                    b.name = lang_concat_adj(sizes[size], _("Pulser"));
                }
                b.cannon->color         = projectile;
                b.cannon->range         = 600 + 300 * size;
                b.cannon->damage        = 30 * size * size;
                b.cannon->explosive     = 0;
                b.cannon->roundsPerSec  = 5;
                b.cannon->muzzleVel     = 1000;
                b.name                  = _("Plasma Condenser");
            }
        }
    }

    {   SerialBlock& b = materializeBlock(NULL, 11100, 11, weapon, wline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.replicateTime = 2.f;
        b.fillColor1    = weapon1;
        b.launcherSpeed = 300;
        b.name          = _("Antimatter Missle Launcher");
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateBlock->shape     = SHAPE_MISSILE;
        b.replicateBlock->scale     = 2;
        b.replicateBlock->thrusterForce  = 6000.f;
        b.replicateBlock->torquerTorque  = 12000;
        b.replicateBlock->durability      = 1.f;
        b.replicateBlock->lifetime       = 10;
        b.replicateBlock->explodeDamage  = 90;
        b.replicateBlock->explodeRadius  = 20;
        b.replicateBlock->fillColor      = fill;
        b.replicateBlock->fillColor1     = weapon1;
        b.replicateBlock->thrusterColor  = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->lineColor      = wline; }


    {   SerialBlock& b = materializeBlock(NULL, 11101, 11, weapon, wline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.replicateTime = 2.f;
        b.fillColor1    = weapon1;
        b.launcherSpeed = 300;
        b.scale    = 2;
        b.name          = _("Antimatter Missle Launcher");
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->scale    = 3;
        b.replicateBlock->thrusterForce = 8000.f;
        b.replicateBlock->torquerTorque = 14000;
        b.replicateBlock->durability     = 1.f;
        b.replicateBlock->lifetime      = 15;
        b.replicateBlock->explodeDamage = 120;
        b.replicateBlock->explodeRadius = 40;
        b.replicateBlock->fillColor     = fill;
        b.replicateBlock->fillColor1    = weapon1;
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->lineColor     = wline; }

    {   SerialBlock &b = materializeBlock(NULL, 11102, 11, generator, line,
                                          SHAPE_RHOMBUS_72_108,
                                          Block::GENERATOR);
        b.name                    = _("Power Conduit");
        b.fillColor1              = generator1;
        b.generatorCapacityPerSec = 200; }
    
    {   SerialBlock& b = materializeBlock(NULL, 11103, 11, weapon, wline, SHAPE_ISOTRI_72, 0);
        b.setFeatures(b.features|Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name          = _("Plasma Diffractor");
        b.fillColor1             = weapon2;
        b.cannon->color          = projectile;
        b.cannon->range          = 400;
        b.cannon->damage         = 10;
        b.cannon->explosive      = EExplosive::PROXIMITY;
        b.cannon->explodeRadius  = 30;
        b.cannon->roundsPerSec   = 7;
        b.cannon->burstyness     = 0.3;
        b.cannon->roundsPerBurst = 3;
        b.cannon->muzzleVel      = 1000; }

    {   SerialBlock &b = materializeBlock(NULL, 11104, 11, fill, line,
                                          SHAPE_RECT_ROOT, Block::ROOT);
        b.name                    = _("Root");
        b.scale              = 2;
        b.fillColor1              = fill1; }

    {   SerialBlock &b = materializeBlock(NULL, 11105, 11, lightning, wline,
                                          SHAPE_PENTAGON, Block::FACTORY|Block::TRACTOR);
        b.name         = _("Factory");
        b.tractorRange = 700.f;
        b.scale   = 2;
        b.fillColor1   = lightning1; }
}


static void factionTwelve()
{
    const uint fill         = 0x514644;
    const uint fill1        = 0x3A2B26;
    const uint thrust       = 0x7D7173;
    const uint thrust1      = 0x4F3739;
    const uint line         = 0xBAADB6;
    const uint afill        = 0xCA8608;
    const uint afill1       = 0x9B7000;
    const uint aline        = 0xFCA904;
    const uint athrust      = 0xC97500;
    const uint athrust1     = 0x855700;
    const uint command      = 0xF9DCBE;
    const uint command1     = 0xD2AA30;
    const uint weapon       = 0x6F5250;
    const uint weapon1      = 0x794937;
    const uint weapon2      = 0x4A3B38;
    const uint weaponline   = 0xBDA09A;
    const uint projectile   = 0xEE5245;
    const uint thrustColor  = 0xA3372A;
    const uint thrustColor1 = 0xEE5245;
    const uint athrustColor  = 0xFE6225;
    const uint athrustColor1 = 0xA3276A;

    const uint shapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI_22_5L, SHAPE_RIGHT_TRI_22_5R,
                            SHAPE_RIGHT_TRI, SHAPE_DISH_THRUSTER, SHAPE_ADAPTER, SHAPE_RECT_V1, 
                            SHAPE_RECT_ROOT };
    const uint maxsize = 4;
    const BlockColors colors[] = { { fill, fill1, line }, { afill, afill1, aline } };
    
    {   SerialBlock &b = materializeBlock(NULL, 12000, 12, command, line, SHAPE_COMMAND,
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Command");
        b.fillColor1              = command1;
        b.capacity                = 1000;
        b.generatorCapacityPerSec = 4000;
        b.durability               = 3.f; }

    for (uint size=1; size<maxsize; size++) {
        for (uint sidx = 0; sidx < arraySize(shapes); sidx++) {
            if (!hasShapeSpec(shapes[sidx], size))
                continue;
            const BlockShape shape = (BlockShape) shapes[sidx];

            for (uint i=0; i<arraySize(colors); i++) {
            
                SerialBlock& b = materializeBlock(NULL, 12001 + maxsize * arraySize(colors) * sidx + size * arraySize(colors) + i, 
                                                  12, colors[i].fill, colors[i].line, shape, 0);
                b.fillColor1 = colors[i].fill1;
                b.scale = size;
                b.durability  = 1.5f;

                if (i == 1) {
                    b.durability *= 1.5f;
                    b.density *= 1.5f;
                    b.name = _("Heavy Bodywork");
                } else {
                    b.name = _("Light Bodywork");
                }

                if (shape == SHAPE_THRUSTER ||
                    shape == SHAPE_THRUSTER_PENT ||
                    shape == SHAPE_DISH_THRUSTER)
                {
                    b.features      |= Block::THRUSTER;
                    b.thrusterForce  = 15000 * size * size;

                    if (i == 1) {
                        b.thrusterBoost *= 4.f;
                        b.thrusterBoostTime = 1.f;
                        b.thrusterForce /= 2.f;
                        b.fillColor = athrust;
                        b.fillColor1 = athrust1;
                        b.thrusterColor = athrustColor;
                        b.thrusterColor = athrustColor1;
                        b.name = _("Maneuver Thruster");
                        b.blurb = _("Highly responsive low impulse thruster");
                    } else {
                        b.fillColor1 = thrust1;
                        b.fillColor = thrust;
                        b.thrusterColor  = thrustColor;
                        b.thrusterColor1  = thrustColor1;
                        b.name = _("Impulse Thruster");
                        b.blurb = _("High impulse thruster");
                    }
                }
                else if (shape == SHAPE_RECT_ROOT)
                {
                    b.name = _("Root");
                    b.features |= Block::ROOT;
                }
            }
        }
    }

    for (uint i=1; i<3; i++)
    {
        SerialBlock& b = materializeBlock(NULL, 12100 + i, 12, weapon, weaponline,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name           = _("Shard Launcher");
        b.blurb          = _("Launches explosive spinners");
        b.replicateTime  = 0.5f;
        b.fillColor1     = weapon1;
        b.launcherSpeed  = 600 + 100 * i;
        b.launcherAngVel = 12;
        b.scale     = i;
        b.replicateBlock->name = _("Shard");
        b.replicateBlock->setFeatures(Block::EXPLODE);
        b.replicateBlock->shape    = SHAPE_RHOMBUS_36_144;
        b.replicateBlock->scale    = i;
        b.replicateBlock->durability     = 1.f;
        b.replicateBlock->lifetime      = 5;
        b.replicateBlock->explodeDamage = 50 * i;
        b.replicateBlock->explodeRadius = 20 * i;
        b.replicateBlock->fillColor     = fill;
        b.replicateBlock->fillColor1    = fill1;
        b.replicateBlock->lineColor     = projectile; 
    }

    const char* sizes[] = {"Small", "Medium", "Large"};
    for (uint i=1; i<3; i++)
    {
        SerialBlock& b = materializeBlock(NULL, 12105 + i, 12, weapon, weaponline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.replicateTime  = 4.f;
        b.fillColor1     = weapon1;
        b.launcherSpeed  = 200;
        b.scale     = i;
        b.name           = _("Laser Drone Launcher");
        b.replicateBlock->setFeatures(Block::TURRET|Block::LASER|Block::TORQUER|Block::THRUSTER|Block::COMMAND|Block::FREERES|Block::GENERATOR);
        b.replicateBlock->name      = lang_concat_adj(sizes[i], _("Laser Drone"));
        b.replicateBlock->shape     = SHAPE_DISH_MISSILE;
        b.replicateBlock->scale     = i+2;
        b.replicateBlock->thrusterForce  = 20000.f;
        b.replicateBlock->torquerTorque  = 16000;
        b.replicateBlock->thrusterColor  = thrustColor;
        b.replicateBlock->thrusterColor1 = thrustColor1;
        b.replicateBlock->durability     = 1.f;
        b.replicateBlock->lifetime       = 15;
        b.replicateBlock->laser->color   = ALPHAF(0.5f)|projectile;
        b.replicateBlock->laser->range   = 200 + 100 * i;
        b.replicateBlock->laser->damage  = 300 + 100 * i;
        b.replicateBlock->laser->width   = 4;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->fillColor      = fill;
        b.replicateBlock->fillColor1     = fill1;
        b.replicateBlock->lineColor      = aline;
    }

    {
        SerialBlock& b = materializeBlock(NULL, 12108, 12, weapon, weaponline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.replicateTime  = 7.f;
        b.fillColor1     = thrust;
        b.launcherSpeed  = 200;
        b.scale          = 1;
        b.name           = _("Anchor Launcher");
        SerialBlock &rb = *b.replicateBlock;
        rb.setFeatures(Block::LASER|Block::TURRET|Block::TORQUER|Block::THRUSTER|Block::COMMAND|Block::FREERES|Block::GENERATOR|Block::SHIELD);
        rb.name      = _("Anchor Drone");
        rb.shape     = SHAPE_COMMAND_MISSILE;
        rb.scale     = 1;
        rb.thrusterForce  = 10000.f;
        rb.torquerTorque  = 50000;
        rb.thrusterColor  = thrustColor;
        rb.thrusterColor1 = thrustColor1;
        rb.durability     = 2.f;
        rb.density        = 15.f * kComponentDensity;
        rb.lifetime       = 30;
        rb.laser->color   = ALPHAF(0.5f)|weaponline;
        rb.laser->range   = 500;
        rb.laser->damage  = 5.f;
        rb.laser->immobilizeForce  = 50000.f;
        rb.laser->linearForce  = -20000.f;
        rb.laser->width   = 4;
        rb.generatorCapacityPerSec = 40;
        rb.fillColor      = fill;
        rb.fillColor1     = fill1;
        rb.lineColor      = weaponline;
        rb.shield->radius   = 40;
        rb.shield->strength = 200;
        rb.shield->regen    = 10;
    }

    for (uint i=1; i<maxsize; i++)
    {
        SerialBlock& b = materializeBlock(NULL, 12110 + i, 12, weapon, weaponline, SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                   = _("PD Subsystem");
        b.fillColor1             = weapon2;
        b.scale             = i;
        b.cannon->color          = projectile;
        b.cannon->range          = 500 + 100 * i;
        b.cannon->damage         = 5 + 2 * i;
        b.cannon->explosive      = i > 1 ? EExplosive::PROXIMITY : EExplosive::ZERO;
        if (i > 1)
            b.cannon->explodeRadius  = 10.f;
        b.cannon->roundsPerSec   = i > 1 ? 5 : 7;
        b.cannon->burstyness     = 0.3;
        b.cannon->roundsPerBurst = 5;
        b.cannon->muzzleVel      = 1000;
    }

    for (uint i=1; i<maxsize-1; i++)
    {
        SerialBlock& b  = materializeBlock(NULL, 12120 + i, 12, weapon, weaponline, SHAPE_CANNON, Block::LASER);
        b.name          = _("Quantum Doom Beam");
        b.fillColor1    = weapon2;
        b.scale    = i;
        b.laser->color  = ALPHAF(0.5f)|projectile;
        b.laser->range  = 200 + 100 * i;
        b.laser->damage = 1000 + 500 * i;
        if (1 > 1)
            b.laser->immobilizeForce = 15000;
        b.laser->width  = 7; 
    }

    {   SerialBlock &b = materializeBlock(NULL, 12130, 12, command, line, SHAPE_COMMAND,
                                          Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::PHOTOSYNTH|Block::ASSEMBLER);
        b.name                    = _("Subcommand");
        b.generatorCapacityPerSec = 1000;
        b.capacity                = 1000;
        b.scale              = 3;
        b.fillColor1              = command1;
        b.photosynthPerSec        = 1.f;
        b.durability               = 2.f; }

    {   SerialBlock &b  = materializeBlock(NULL, 12131, 12, afill, aline,
                                           SHAPE_SQUARE, Block::FACTORY);
        b.name       = _("Factory");
        b.fillColor1 = thrustColor;
        b.scale = 4; }

    {   SerialBlock &b  = materializeBlock(NULL, 12132, 12, afill, aline,
                                           SHAPE_SQUARE, Block::SHIELD);
        b.name       = _("Shield");
        b.fillColor1 = weapon1;
        b.scale = 3;
        b.shield->radius   = 200;
        b.shield->strength = 1200;
        b.shield->regen    = 250; }

    {   SerialBlock &b  = materializeBlock(NULL, 12133, 12, afill, line,
                                           SHAPE_SQUARE, Block::TRACTOR);
        b.name = _("Tractor Beam");
        b.fillColor1 = fill1;
        b.scale = 1; }

    {   SerialBlock &b  = materializeBlock(NULL, 12134, 12, afill, aline,
                                           SHAPE_SQUARE, Block::GENERATOR);
        b.name = _("Generator/Storage");
        b.fillColor1 = fill1;
        b.scale = 2;
        b.generatorCapacityPerSec = 1000;
        b.capacity                = 1000; }
}

static void factionThirteen()
{
    const uint fill        = 0x677606;
    const uint fill1       = 0x424512;
    const uint line        = 0x879606;
    const uint command     = 0x98a606;
    const uint command1    = 0x08a656;
    const uint wfill       = 0x778036;
    const uint wfill1      = 0x92a202;
    const uint wline       = 0xe7f606;
    const uint thruster    = 0x677646;
    const uint thruster1   = 0x507646;
    const uint thrust      = 0xE56C65;
    const uint thrust1     = 0x764593;
    const uint projectile  = 0xb7ff00;
    const uint projectile1 = 0xcfff0f;

    { SerialBlock& b = materializeBlock(NULL, 13000, 13, command, line, SHAPE_SEED_3,
                                        Block::SEED|Block::COMMAND|Block::PHOTOSYNTH|
                                        Block::THRUSTER|Block::TORQUER|Block::GENERATOR|Block::REGROWER);
        b.name             = _("Seed");
        b.fillColor1       = command1;
        b.thrusterForce    = 2000.f;
        b.torquerTorque    = 4000.f;
        b.capacity         = 100.f;
        b.durability       = 3.f;
        b.photosynthPerSec = 1;
        b.seedLifetime     = 0.f;
        b.launchLifetime   = 30.f;
        b.thrusterColor    = thrust;
        b.thrusterColor1   = thrust1;
        b.generatorCapacityPerSec = 150.f; }

    { SerialBlock& b = materializeBlock(NULL, 13001, 13, command, line, SHAPE_HEPTAGON,
                                        Block::COMMAND|Block::PHOTOSYNTH|Block::GENERATOR);
        b.name             = _("Pod Command");
        b.scale       = 3;
        b.fillColor1       = command1;
        b.capacity         = 1000.f;
        b.photosynthPerSec = 5;
        b.generatorCapacityPerSec = 1000.f;}

    { SerialBlock& b = materializeBlock(NULL, 13002, 13, command, line, SHAPE_OCTAGON,
                                        Block::COMMAND|Block::PHOTOSYNTH);
        b.name             = _("Pod Command");
        b.scale       = 3;
        b.fillColor1       = command1;
        b.capacity         = 1000.f;
        b.photosynthPerSec = 5; }

    const BlockShape shapes[] = { SHAPE_GEM_1, SHAPE_GEM_2, SHAPE_GEM_3, SHAPE_GEM_4,
                                  SHAPE_HEPTAGON, SHAPE_OCTAGON, SHAPE_RECT_V1, SHAPE_ISOTRI_25, 
                                  SHAPE_THRUSTER };
    for (uint i=1; i<4; i++)
    {
        for (uint j=0; j<arraySize(shapes); j++)
        {
            if (!hasShapeSpec(shapes[j], i))
                continue;
            SerialBlock &b = materializeBlock(NULL, 13010 + 4 * arraySize(shapes) * (i-1) + j, 13, fill, line, shapes[j], 0);
            b.fillColor1  = fill1;
            b.name        = _("Polyp");
            b.scale  = i;

            if (shapes[j] == SHAPE_THRUSTER)
            {
                b.features       |= Block::THRUSTER;
                b.thrusterForce   = 22000 * i;
                b.fillColor       = thruster;
                b.fillColor1      = thruster1;
                b.thrusterColor   = thrust;
                b.thrusterColor1  = thrust1;
            }
        }
    }
    
    { SerialBlock &b = materializeBlock(NULL, 13094, 13, fill, line, SHAPE_SQUARE, 0);
        b.fillColor1  = fill1;
        b.scale = 2;
        b.name        = _("Polyp");}
    
    { SerialBlock &b = materializeBlock(NULL, 13095, 13, fill, line, SHAPE_SQUARE, 0);
        b.fillColor1  = fill1;
        b.name        = _("Polyp");}

    { SerialBlock &b = materializeBlock(NULL, 13096, 13, fill, line, SHAPE_RECT_QUARTER, 0);
        b.fillColor1  = fill1;
        b.scale  = 4;
        b.name        = _("Polyp");}

    { SerialBlock &b = materializeBlock(NULL, 13097, 13, fill, line, SHAPE_RECT_QUARTER, 0);
        b.fillColor1  = fill1;
        b.scale  = 6;
        b.name        = _("Polyp");}

    { SerialBlock &b = materializeBlock(NULL, 13098, 13, fill, line, SHAPE_HEPTAGON, 0);
        b.fillColor1  = fill1;
        b.scale  = 3;
        b.name        = _("Polyp");}

    { SerialBlock &b = materializeBlock(NULL, 13099, 13, fill, line, SHAPE_ADAPTER, 0);
        b.fillColor1  = fill1;
        b.name        = _("Polyp");}

    for (uint i=1; i<4; i++)
    {
        SerialBlock &b = materializeBlock(NULL, 13100 + i, 13, wfill, wline, 
                                          i == 3 ? SHAPE_ISOTRI_3 : SHAPE_ISOTRI_6, Block::LASER|Block::MELEE);
        b.fillColor1   = wfill1;
        b.name         = _("Light Spine");
        b.scale   = i == 3 ? 9 :  2 + i;
        b.durability   = 10.f;
        b.laser->damage  = 100 * i;
        b.laser->range   = 700 + 100 * i;
        b.laser->width   = 2 + i * i;
        b.laser->color   = ALPHAF(0.5)|projectile1;
        b.laser->linearForce = -100000;
    }

    const BlockShape lshapes[] = { SHAPE_GEM_3_LAUNCHER, SHAPE_GEM_4_LAUNCHER,
                                   SHAPE_HEPTAGON_LAUNCHER };
    for (uint i=0; i<3; i++)
    {
        for (uint j=0; j<arraySize(lshapes); j++)
        {
            uint tt = i;
            if (i == 2) {
                if (lshapes[j] == SHAPE_HEPTAGON_LAUNCHER)
                    tt = 1;
                else
                    continue;
            }
                

            SerialBlock &b = materializeBlock(NULL, 13110 + 3 * i * arraySize(lshapes) + j, 13, wfill, wline,
                                              lshapes[j], Block::LAUNCHER);
            b.name           = _("Thistle");
            b.fillColor1     = wfill1;
            b.scale     = i + 1; 
            b.replicateBlock->setFeatures(Block::THRUSTER|Block::EXPLODE);
            b.replicateTime                 = 0.5f;
            b.replicateBlock->shape    = SHAPE_ISOTRI_25_MISSILE;
            b.replicateBlock->launchSpeed   = 500.f;
            b.replicateBlock->scale    = tt + 1;
            b.replicateBlock->thrusterForce = 2000.f + 500.f * tt;
            b.replicateBlock->durability     = 0.2f;
            b.replicateBlock->lifetime      = 5;
            b.replicateBlock->explodeDamage = 50 + 20 * tt;
            b.replicateBlock->explodeRadius = 20 + 5 * tt;
            b.replicateBlock->fillColor     = projectile;
            b.replicateBlock->fillColor1    = projectile1;
            b.replicateBlock->lineColor     = wline;
            b.replicateBlock->thrusterColor = thrust;
        }
    }

}

static void factionFourteen()
{
    const uint fill        = 0x404040;
    const uint fill1       = 0x606060;
    const uint cfill       = 0xa0a0a0;
    const uint cfill1      = 0xa0b0b0;
    const uint cfill2      = 0x809090;
    const uint afill       = 0x00a0e0;
    const uint afill1      = 0x00e0a0;
    const uint aline       = 0x00ffff;
    const uint bfill       = 0xe05000;
    const uint bfill1      = 0xe0a000;
    const uint bline       = 0xff5030;
    const uint line        = 0xffffff;
    const uint thrustColor = 0x8fffff;

    {  SerialBlock &b = materializeBlock(NULL, 14000, 14, cfill, line, SHAPE_COMMAND,
                                         Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Command");
        b.scale              = 1;
        b.fillColor1              = cfill1;
        b.capacity                = 1000;
        b.generatorCapacityPerSec = 4000;
        b.durability               = 4.f; }

    {   SerialBlock &b = materializeBlock(NULL, 14100, 14, afill, aline, SHAPE_RIGHT_TRI, Block::LASER|Block::TURRET|Block::CHARGING);
        b.name                    = _("Burst Laser");
        b.fillColor1              = afill1;
        b.scale              = 2;
        b.turretSpeed             = 2;
        b.laser->pulsesPerSec       = 2.f;
        b.laser->burstyness         = 0.5f;
        b.laser->pulsesPerBurst     = 2;
        b.laser->pulseAvailability  = 0.3f;
        b.laser->explosive         = EExplosive::ENABLED;
        b.laser->explodeRadius     = 20.f;
        b.laser->damage             = 300;
        b.laser->range              = 1800;
        b.laser->width              = 8;
        b.laser->decay              = 1.f;
        b.laser->color              = ALPHAF(0.5f)|afill1; }

    const BlockShape shapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI, SHAPE_THRUSTER, SHAPE_ADAPTER };
    const uint maxsize = 4;

    const BlockColors colors[] = { { fill, fill1, line }, { cfill, cfill2, line },
                                   { afill, afill1, aline}, { bfill, bfill1, bline} };

    for (uint size=1; size<maxsize; size++) {
        for (uint sidx = 0; sidx < arraySize(shapes); sidx++) {
            if (!hasShapeSpec(shapes[sidx], size))
                continue;
            const BlockShape shape = (BlockShape) shapes[sidx];

            for (uint i=0; i<arraySize(colors); i++) {
            
                SerialBlock& b = materializeBlock(NULL, 14001 + maxsize * arraySize(colors) * sidx + size * arraySize(colors) + i, 
                                                  14, colors[i].fill, colors[i].line, shape, 0);
                b.fillColor1 = colors[i].fill1;
                b.scale = size;
                b.growRate   = 20.f;

                if (shape == SHAPE_THRUSTER ||
                    shape == SHAPE_THRUSTER_PENT ||
                    shape == SHAPE_DISH_THRUSTER)
                {
                    b.features      |= Block::THRUSTER;
                    b.thrusterForce  = 10000 * size * size;
                    b.thrusterColor  = thrustColor;
                }
            }
        }
    }
}


static void factionFifteen()
{
    const uint fill        = 0x404840;
    const uint fill1       = 0x282020;
    const uint line        = 0x4d9251;
    const uint cfill       = 0x808080;
    const uint cfill1      = 0x505050;
    const uint cline       = 0xdfe0e2;
    const uint afill       = 0x96bc2a;
    const uint afill1      = 0x378b2f;
    const uint aline       = 0xf4f0c4;
    const uint shot        = 0x97ee2f;
    const uint thrustColor = 0x54f5c4;

    const uint copper      = 0xe8a97e;
    const uint copper1     = 0x775339;
    const uint copperline  = 0x5d0d02;
    const uint special     = 0x065977;

    {  SerialBlock &b = materializeBlock(NULL, 15000, 15, afill, cline, SHAPE_COMMAND,
                                         Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Command");
        b.scale              = 1;
        b.fillColor1              = cfill;
        b.capacity                = 1000;
        b.generatorCapacityPerSec = 1000;
        b.tractorRange            = 700.f;
        b.durability              = 4.f; }

    {   SerialBlock &b   = materializeBlock(NULL, 15001, 15, afill, aline, 
                                            SHAPE_CANNON2, Block::LASER|Block::CHARGING);
        b.name           = _("Gauss Beam");
        b.blurb          = _("Devastating pulse weapon");
        b.fillColor1     = afill1;
        b.scale     = 1;
        b.chargeMin      = 0.3f;
        b.laser->color   = ALPHAF(0.5f)|shot;
        b.laser->range   = 1500;
        b.laser->damage  = 1000;
        b.laser->width   = 8; 
        b.durability     = 2.f; }

    {   SerialBlock &b   = materializeBlock(NULL, 15002, 15, afill, aline, 
                                            SHAPE_SQUARE, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.name                  = _("Plasma Bolt");
        b.blurb                 = _("Slow, high damage defense turret");
        b.fillColor1            = afill1;
        b.scale            = 1;
        b.durability            = 2.f;
        b.cannon->color         = shot;
        b.cannon->range         = 1500;
        b.cannon->damage        = 50;
        b.cannon->explosive     = EExplosive::PROXIMITY;
        b.cannon->explodeRadius = 15.f;
        b.cannon->roundsPerSec  = 0.5f;
        b.cannon->muzzleVel     = 1200;
        b.cannon->spread        = 0.0;
        b.cannon->burstyness    = 0.0; }

    {   SerialBlock &b = materializeBlock(NULL, 15003, 15, cfill, cline,
                                          SHAPE_RECT_LAUNCHER, Block::LAUNCHER);
        b.name = _("Flurry Missiles");
        b.blurb = _("Rapid-fire guided missile launcher");
        b.fillColor1                    = cfill1;
        b.launcherPower                 = 50;
        b.launcherOutSpeed              = 50.f; 
        b.durability     = 2.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE);
        b.replicateTime                 = 0.5;
        b.replicateBlock->shape    = SHAPE_MISSILE;
        b.replicateBlock->thrusterForce = 2400.f;
        b.replicateBlock->torquerTorque = 4000.f;
        b.replicateBlock->durability     = 1.f;
        b.replicateBlock->lifetime      = 4;
        b.replicateBlock->explodeDamage = 10;
        b.replicateBlock->explodeRadius = 20;
        b.replicateBlock->fillColor     = cfill1;
        b.replicateBlock->fillColor1    = afill;
        b.replicateBlock->lineColor     = cline;
        b.replicateBlock->thrusterColor = thrustColor; }

    {   SerialBlock &b   = materializeBlock(NULL, 15004, 15, afill, aline, 
                                            SHAPE_SQUARE, Block::CANNON|Block::TURRET);
        b.name = _("Annihilator");
        b.blurb = _("Slow but powerful gun turret");
        b.fillColor1             = afill1;
        b.scale             = 2;
        b.durability     = 2.f;
        b.cannon->color          = shot;
        b.cannon->range          = 1500;
        b.cannon->damage         = 100;
        b.cannon->explosive      = EExplosive::ENABLED;
        b.cannon->explodeRadius  = 35.f;
        b.cannon->roundsPerSec   = 0.5;
        b.cannon->muzzleVel      = 1000;
        b.cannon->spread         = 0.0;
        b.cannon->burstyness     = 0.0; }

    {   SerialBlock &b   = materializeBlock(NULL, 15005, 15, afill, aline, 
                                            SHAPE_SQUARE, Block::LASER|Block::TURRET|Block::CHARGING);
        b.name = _("Obliterator");
        b.blurb = _("Obscenely powerful death laser");
        b.fillColor1    = special;
        b.scale    = 3;
        b.turretSpeed   = 1;
        b.chargeMaxTime = 3.f;
        b.chargeMin     = 0.5f;
        b.durability     = 2.f;
        b.laser->color  = ALPHAF(0.8f)|shot;
        b.laser->explosive = EExplosive::ENABLED;
        b.laser->explodeRadius = 40.f;
        b.laser->decay  = 0.35f;
        b.laser->range  = 1800;
        b.laser->damage = 2000;
        b.laser->width  = 20; }

    {   SerialBlock &b  = materializeBlock(NULL, 1506, 15, afill, aline,
                                           SHAPE_SQUARE, Block::FACTORY);
        b.name       = _("Factory");
        b.fillColor1 = thrustColor;
        b.scale = 4; }

    {   SerialBlock &b  = materializeBlock(NULL, 1508, 15, afill, aline,
                                           SHAPE_SQUARE, Block::GENERATOR);
        b.name                    = _("Reactor");
        b.generatorCapacityPerSec = 1000;
        b.fillColor1              = fill;
        b.scale              = 2; }

    {   SerialBlock &b  = materializeBlock(NULL, 1507, 15, cfill, cline,
                                           SHAPE_RECT_LONG, 0);
        b.name       = _("Depot");
        b.blurb      = _("Resource storage");
        b.capacity   = 200.f;
        b.fillColor1 = fill;
        b.scale = 3; }

    {   SerialBlock &b = materializeBlock(NULL, 15008, 15, cfill, cline,
                                          SHAPE_RECT_LAUNCHER1, Block::LAUNCHER);
        b.name = _("Nuclear Option");
        b.blurb = _("Heavily armored death launcher");
        b.fillColor1                    = cfill1;
        b.scale                    = 3;
        b.launcherPower                 = 50;
        b.launcherOutSpeed              = 50.f;
        b.durability     = 2.f;
        b.replicateBlock->setFeatures(Block::COMMAND|Block::THRUSTER|Block::TORQUER|Block::EXPLODE|Block::CANNON|Block::TURRET|Block::FREERES|Block::GENERATOR|Block::AUTOFIRE);
        b.replicateTime                 = 5.f;
        b.replicateBlock->shape    = SHAPE_MISSILE_SHORT;
        b.replicateBlock->scale    = 4;
        b.replicateBlock->thrusterForce = 22000.f;
        b.replicateBlock->torquerTorque = 34000.f;
        b.replicateBlock->durability     = 10.f;
        b.replicateBlock->lifetime      = 30;
        b.replicateBlock->explodeDamage = 1000;
        b.replicateBlock->explodeRadius = 600;
        b.replicateBlock->fillColor     = cfill1;
        b.replicateBlock->fillColor1    = afill;
        b.replicateBlock->lineColor     = cline;
        b.replicateBlock->thrusterColor = thrustColor;
        b.replicateBlock->generatorCapacityPerSec = 40;
        b.replicateBlock->cannon->color           = shot;
        b.replicateBlock->cannon->range           = 600;
        b.replicateBlock->cannon->damage          = 15;
        b.replicateBlock->cannon->roundsPerSec    = 4;
        b.replicateBlock->cannon->muzzleVel       = 600;
        b.replicateBlock->cannon->spread          = 0.05;
        b.replicateBlock->cannon->burstyness      = 0.6;
        b.replicateBlock->cannon->roundsPerBurst  = 4;}

    {  SerialBlock &b = materializeBlock(NULL, 15009, 15, afill, cline, SHAPE_SQUARE,
                                         Block::TRACTOR);
        b.name                    = _("Harvester");
        b.scale              = 1;
        b.tractorRange            = 900.f;
        b.fillColor1              = cfill;
        b.capacity                = 1000;}

    const BlockShape ashapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI2L, SHAPE_RIGHT_TRI2R, SHAPE_ADAPTER, SHAPE_RIGHT_TRI,
                                   SHAPE_RECT_V1 };

    const uint armormax = 3;
    for (uint size=1; size<armormax; size++) {
        for (uint sidx = 0; sidx < arraySize(ashapes); sidx++) {
            if (!hasShapeSpec((BlockShape)ashapes[sidx], size))
                continue;
            const BlockShape shape = (BlockShape) ashapes[sidx];
            SerialBlock& b = materializeBlock(NULL, 15030 + (size-1) * arraySize(ashapes) + sidx,
                                              15, copper, copperline, shape, 0);
            b.name = _("Armor Plate");
            b.fillColor1 = copper1;
            b.scale = size;
            b.growRate   = 5.f;
            b.durability = 10.f;
            b.density    = 2.f * kComponentDensity;

            if (shape == SHAPE_RIGHT_TRI2L || shape == SHAPE_RIGHT_TRI2R)
            {
                b.name = _("Armor Spike");
                b.features |= Block::MELEE;
            }
        }
    }

    const uint maxsize = 4;
    const BlockShape shapes[] = { SHAPE_SQUARE, SHAPE_RIGHT_TRI2L, SHAPE_RIGHT_TRI2R, SHAPE_ADAPTER, SHAPE_RIGHT_TRI,
                                  SHAPE_DISH_THRUSTER  };

    const BlockColors colors[] = { { fill, fill1, line }, { cfill, cfill1, cline } };

    for (uint size=1; size<maxsize; size++) {
        for (uint sidx = 0; sidx < arraySize(shapes); sidx++) {
            if (!hasShapeSpec((BlockShape)shapes[sidx], size))
                continue;
            const BlockShape shape = (BlockShape) shapes[sidx];

            for (uint i=0; i<arraySize(colors); i++) {
            
                SerialBlock& b = materializeBlock(NULL, 15100 + maxsize * arraySize(colors) * sidx + size * arraySize(colors) + i, 
                                                  15, colors[i].fill, colors[i].line, shape, 0);
                b.name = _("Segment");
                b.fillColor1 = colors[i].fill1;
                b.scale = size;
                b.growRate   = 20.f;
                b.durability  = 2.f;

                if (i == 1)
                {
                    b.blurb = _("Core hull");
                    b.durability *= 2.f;
                    b.density *= 1.2f;
                }
                else
                {
                    b.blurb = _("Auxiliary hull");
                }

                if (shape == SHAPE_DISH_THRUSTER)
                {
                    b.name = _("Thruster");
                    b.blurb = _("Thrust vector unit");
                    b.features      |= Block::THRUSTER;
                    b.thrusterForce  = 8000 * size * size;
                    b.thrusterColor  = thrustColor;
                }
            }
        }
    }
}


static void factionSixteen()
{
    const uint fill        = 0x48DBD5;
    const uint fill1       = 0x1498B8;
    const uint line        = 0x91F5E8;
    const uint cfill       = 0xF6441E;
    const uint cfill1      = 0xEE8224;
    const uint cline       = 0xFAE5D6;
    const uint projectile  = 0xFAD118;
    const uint thrustColor = 0x0884A7;

    {  SerialBlock &b = materializeBlock(NULL, 16000, 16, fill, line, SHAPE_COMMAND,
                                         Block::COMMAND|Block::GENERATOR|Block::TRACTOR|Block::ASSEMBLER);
        b.name                    = _("Command");
        b.scale              = 1;
        b.fillColor1              = fill;
        b.capacity                = 100;
        b.generatorCapacityPerSec = 40;
        b.durability               = 0.2f; }

    {   SerialBlock &b   = materializeBlock(NULL, 16001, 16, cfill, cline,
                                            SHAPE_OCTAGON, Block::CANNON|Block::TURRET|Block::AUTOFIRE);
        b.fillColor1             = cfill1;
        b.scale             = 1;
        b.cannon->color          = projectile;
        b.cannon->range          = 1500;
        b.cannon->damage         = 25;
        b.cannon->explosive      = 0;
        b.cannon->roundsPerSec   = 1.f;
        b.cannon->muzzleVel      = 600;
        b.cannon->spread         = 0.0;
        b.cannon->burstyness     = 0.5;
        b.cannon->roundsPerBurst = 3; }

    const BlockShape shapes[] = { SHAPE_SQUARE, SHAPE_ADAPTER, SHAPE_RIGHT_TRI,
                                  SHAPE_RECT_V1, SHAPE_OCTAGON, SHAPE_THRUSTER };
    const uint maxsize = 3;
    const BlockColors colors[] = { { fill, fill1, line }, { cfill, cfill1, cline } };

    for (uint size=1; size<maxsize; size++) {
        for (uint sidx = 0; sidx < arraySize(shapes); sidx++) {
            if (!hasShapeSpec((BlockShape)shapes[sidx], size))
                continue;
            const BlockShape shape = (BlockShape) shapes[sidx];

            for (uint i=0; i<arraySize(colors); i++) {
            
                SerialBlock& b = materializeBlock(NULL, 16100 + maxsize * arraySize(colors) * sidx + size * arraySize(colors) + i, 
                                                  16, colors[i].fill, colors[i].line, shape, 0);
                b.fillColor1 = colors[i].fill1;
                b.scale = size;
                b.durability  = 0.1f;

                if (shape == SHAPE_THRUSTER)
                {
                    b.features      |= Block::THRUSTER;
                    b.thrusterForce  = 1000 * size * size;
                    b.thrusterColor  = thrustColor;
                }
            }
        }
    }
}

static void createTileAsteroidBlocks()
{
    const int scale = 9;
    const int sides[] = { 3, 4, 5, 6, 7, 8, 9 };

    for (uint i=0; i<arraySize(sides); i++)
    {
        for (uint j=0; j<3; j++)
        {
            SerialBlock& b = materializeBlock(NULL, 10100 + 10 * sides[i] + j, 100,
                                              rgbf2rgb(float3(0.2f + 0.1f * j)),
                                              rgbf2rgb(float3(0.6f)), 
                                              sidesShape(sides[i]),
                                              Block::ENVIRONMENTAL);               
            b.scale = (sides[i] == 8) ? (scale + 1) : scale;
            b.fillColor1 = rgbf2rgb(float3(0.2f + 0.1f * ((j + 1) % 3)));
            // b.fillColor1 = b.fillColor;
            b.durability = 0.5f;
            // b.armor = kAsteroidArmor;
            b.density = kAsteroidDensity;
        }
    }

    // target/tutorial block
    {   SerialBlock &b = materializeBlock(NULL, 10099, 100, 0x333333, 0xff0000, SHAPE_SQUARE, 0);
        b.durability = 0.5f; }
    {   SerialBlock &b = materializeBlock(NULL, 10098, 100, 0xff0000, 0xff0000, SHAPE_SQUARE, Block::COMMAND|Block::GENERATOR);
        b.durability = 0.5f;
        b.generatorCapacityPerSec = 40.f; }


    struct lol { int ident, scale; BlockShape shape; };
    static const lol dat[] = {
        { 10500, 1, SHAPE_SQUARE },
        { 10501, 2, SHAPE_SQUARE },
        { 10502, 3, SHAPE_SQUARE },
        { 10503, 1, SHAPE_RIGHT_TRI2L },
        { 10504, 2, SHAPE_RIGHT_TRI2L },
        { 10505, 3, SHAPE_RIGHT_TRI2L },
        { 10506, 1, SHAPE_RIGHT_TRI2R },
        { 10507, 2, SHAPE_RIGHT_TRI2R },
        { 10508, 3, SHAPE_RIGHT_TRI2R },
        { 10509, 1, SHAPE_RIGHT_TRI },
        { 10510, 2, SHAPE_RIGHT_TRI },
        { 10511, 3, SHAPE_RIGHT_TRI },
    };
    foreach (const lol& l, dat)
    {
        SerialBlock &b = materializeBlock(NULL, l.ident, 100, 0xaaaaaa, 0xeeeeee, l.shape, 0);
        b.durability = 10;
        // b.armor = kAsteroidArmor;
        b.density = 10 * kAsteroidDensity;
        b.scale = l.scale;
    }

    // penrose
    for (int i=0; i<2; i++)
    {
        BlockShape shape = (i == 0) ? SHAPE_RHOMBUS_36_144 : SHAPE_RHOMBUS_72_108;
        SerialBlock &b = materializeBlock(NULL, 10512 + i, 100, rgbf2rgb(float3(0.3f - 0.1f * i)),
                                          rgbf2rgb(float3(0.6f)), shape, Block::ENVIRONMENTAL);
        b.scale = 9;
        b.durability = 0.5f;
        b.density = kAsteroidDensity;
    }
}


