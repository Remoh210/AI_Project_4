#pragma once
#include <glm/glm.hpp>
#include "eBodyType.h"

namespace nPhysics
{
	class iCollisonBody
	{
	public:
		virtual ~iCollisonBody(){}
		virtual void GetAABB(glm::vec3& minOut, glm::vec3& maxOut) = 0;

		inline eBodyType GetShapeType() const { return mBodyType; };

	protected:
		iCollisonBody(eBodyType bodyType) : mBodyType(bodyType) {}
		iCollisonBody(const iCollisonBody& other) {}
		iCollisonBody& operator=(const iCollisonBody& other) { return *this; }

	private:
		eBodyType mBodyType;

	};
}