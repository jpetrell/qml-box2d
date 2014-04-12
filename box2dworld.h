/*
 * box2dworld.h
 * Copyright (c) 2010 Thorbjørn Lindeijer <thorbjorn@lindeijer.nl>
 * Copyright (c) 2011 Joonas Erkinheimo <joonas.erkinheimo@nokia.com>
 * Copyright (c) 2012 Adriano Rezende <atdrez@gmail.com>
 *
 * This file is part of the Box2D QML plugin.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef BOX2DWORLD_H
#define BOX2DWORLD_H

#include <QAbstractAnimation>
#include <QQuickItem>

#include <Box2D.h>

class Box2DContact;
class Box2DFixture;
class Box2DJoint;
class Box2DWorld;
class ContactListener;
class StepDriver;

// TODO: Maybe turn this into a property of the world, though it can't be
// changed dynamically.
static const float pixelsPerMeter = 32.0f; // 32 pixels in one meter
static const float metersPerPixel = 1.0f / pixelsPerMeter;
static const float pixelsPerMeterY = -pixelsPerMeter; // Y-axis inverted
static const float metersPerPixelY = -metersPerPixel;

/**
 * Small utility class to synchronize the stepping with the framerate.
 */
class StepDriver : public QAbstractAnimation
{
    Q_OBJECT

public:
    explicit StepDriver(Box2DWorld *world);

    int duration() const;

protected:
    void updateCurrentTime(int);

private:
    Box2DWorld *mWorld;
};


/**
 * Wrapper class around a Box2D world.
 */
class Box2DWorld : public QQuickItem, b2DestructionListener
{
    Q_OBJECT

    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(float timeStep READ timeStep WRITE setTimeStep NOTIFY timeStepChanged)
    Q_PROPERTY(int velocityIterations READ velocityIterations WRITE setVelocityIterations NOTIFY velocityIterationsChanged)
    Q_PROPERTY(int positionIterations READ positionIterations WRITE setPositionIterations NOTIFY positionIterationsChanged)
    Q_PROPERTY(QPointF gravity READ gravity WRITE setGravity NOTIFY gravityChanged)
    Q_PROPERTY(bool autoClearForces READ autoClearForces WRITE setAutoClearForces NOTIFY autoClearForcesChanged)

public:
    explicit Box2DWorld(QQuickItem *parent = 0);
    ~Box2DWorld();

    float timeStep() const;
    void setTimeStep(float timeStep);

    bool isRunning() const;
    void setRunning(bool running);

    int velocityIterations() const;
    void setVelocityIterations(int iterations);

    int positionIterations() const;
    void setPositionIterations(int iterations);

    QPointF gravity() const;
    void setGravity(const QPointF &gravity);

    bool autoClearForces() const;
    void setAutoClearForces(bool autoClearForces);

    void componentComplete();

    b2World &world();

    // b2DestructionListener interface
    void SayGoodbye(b2Joint *joint);
    void SayGoodbye(b2Fixture *fixture);

    Q_INVOKABLE void step();
    Q_INVOKABLE void clearForces();

signals:
    void initialized();
    void preSolve(Box2DContact * contact);
    void postSolve(Box2DContact * contact);

    void timeStepChanged();
    void velocityIterationsChanged();
    void positionIterationsChanged();
    void gravityChanged();
    void autoClearForcesChanged();
    void runningChanged();
    void stepped();

protected:
    void itemChange(ItemChange, const ItemChangeData &);
    void initializeBodies(QQuickItem *parent);

private:
    b2World mWorld;
    ContactListener *mContactListener;
    float mTimeStep;
    int mVelocityIterations;
    int mPositionIterations;
    bool mIsRunning;
    StepDriver *mStepDriver;
};

/**
 * The amount of time to step through each frame in seconds.
 * By default it is 1 / 60.
 */
inline float Box2DWorld::timeStep() const
{
    return mTimeStep;
}

inline bool Box2DWorld::isRunning() const
{
    return mIsRunning;
}

/**
 * The number of velocity iterations used to process one step.
 * 8 by default.
 */
inline int Box2DWorld::velocityIterations() const
{
    return mVelocityIterations;
}

/**
 * The number of position iterations used to process one step.
 * 3 by default.
 */
inline int Box2DWorld::positionIterations() const
{
    return mPositionIterations;
}

inline bool Box2DWorld::autoClearForces() const
{
    return mWorld.GetAutoClearForces();
}

inline b2World &Box2DWorld::world()
{
    return mWorld;
}

inline void Box2DWorld::clearForces()
{
    mWorld.ClearForces();
}


/**
 * Inverts the y-axis as required for forces and velocities.
 */
inline QPointF invertY(const b2Vec2 &vec)
{
    return QPointF(vec.x, -vec.y);
}

/**
 * Inverts the y-axis as required for forces and velocities.
 */
inline b2Vec2 invertY(const QPointF &vec)
{
    return b2Vec2(vec.x(), -vec.y());
}

/**
 * Converts lengths from Box2D to QML units.
 */
inline float toPixels(float length)
{
    return length * pixelsPerMeter;
}

/**
 * Converts lengths from QML to Box2D units.
 */
inline float toMeters(float length)
{
    return length * metersPerPixel;
}

/**
 * Converts positions and sizes from Box2D to QML coordinates.
 */
inline QPointF toPixels(const b2Vec2 &vec)
{
    return QPointF(vec.x * pixelsPerMeter,
                   vec.y * pixelsPerMeterY);
}

/**
 * Converts positions and sizes from QML to Box2D coordinates.
 */
inline b2Vec2 toMeters(const QPointF &point)
{
    return b2Vec2(point.x() * metersPerPixel,
                  point.y() * metersPerPixelY);
}

/**
 * Converts angles from Box2D to QML values.
 */
inline float toDegrees(float radians)
{
    return -radians * 180 / b2_pi;
}

/**
 * Converts angles from QML to Box2D values.
 */
inline float toRadians(float degrees)
{
    return -degrees * b2_pi / 180;
}


QML_DECLARE_TYPE(Box2DWorld)

#endif // BOX2DWORLD_H
