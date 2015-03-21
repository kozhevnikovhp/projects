#include "particle_shape_vertex.h"
#include <math.h>

void sParticleShapeVertex::RecalcAux()
{
	m_Sine = sin(Incline);
	m_Cosine = cos(Incline);
}


