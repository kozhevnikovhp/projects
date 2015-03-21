#ifndef PARTICLE_SHAPE_VERTEX_H
#define PARTICLE_SHAPE_VERTEX_H

typedef struct sParticleShapeVertex
{
    double x;
    double y;
    double DistanceToParticleCenter;
    double Xk, Yk, R; // center and radius
    double Incline; //in radians
    double m_Sine, m_Cosine;

    void RecalcAux();
} sParticleShapeVertex;

#endif // PARTICLE_SHAPE_VERTEX_H
