#include "texture.h"

namespace rt
{

glm::vec3 RGB_TextureTriangle::getTexel(glm::vec3 pos)
{
	if (tr)
		// calculate barycentric coordinates
		return 10.f * (tr->m_inv * pos);
	else
	{
		VLOG(1) << "RGB_TextureTriangle Triangle class member is empty";
		return glm::vec3(0.f);
	}
}

}