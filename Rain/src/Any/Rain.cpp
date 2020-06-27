#include "Rain.h"
#include "constants.h"
#include "../Struct/Box.h"
#include "../Struct/Point3D.h"
#include "../Struct/Size3D.h"
#include "../Functions/control_flow.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Enums.h"
#include <random>

using glm::identity;
using glm::translate;
using glm::scale;
using glm::vec3;
using glm::mat4;
using std::uniform_real_distribution;

namespace ii887522::Rain
{
	Rain::Rain() : box{ Point3D{ -14.f * aspectRatio, -14.f, -75.f }, Size3D{ 28.f * aspectRatio, 28.f, 74.f } }, dropSize{ .1f },
		randomEngine{ }, speed{ 8.f }
	{
		const uniform_real_distribution dropXRange{
			box.position.x - drops[0u].positions[0u].x * dropSize, box.position.x + box.size.w - drops[0u].positions[1u].x * dropSize
		};
		const uniform_real_distribution dropYRange{
			box.position.y - drops[0u].positions[3u].y * dropSize, box.position.y + box.size.h - drops[0u].positions[0u].y * dropSize,
		};
		const uniform_real_distribution dropZRange{ box.position.z, box.position.z + box.size.l };
		loop(dropCount, [&](const auto i) {
			drops[i].position = Point3D{ dropXRange(randomEngine), dropYRange(randomEngine), dropZRange(randomEngine) };
			drops[i].model = scale(
				translate(identity<mat4>(), vec3{ drops[i].position.x, drops[i].position.y, drops[i].position.z }),
				vec3{ dropSize, dropSize, dropSize }
			);
			return Action::CONTINUE;
		});
	}

	void Rain::fall(const float dt)
	{
		const uniform_real_distribution dropYRange{
			box.position.y + box.size.h - drops[0u].positions[3u].y * dropSize,
			box.position.y + box.size.h * 1.25f - drops[0u].positions[3u].y * dropSize
		};
		loop(dropCount, [&](const auto i) {
			drops[i].position.y -= speed * dt;
			if (drops[i].position.y < box.position.y - drops[i].positions[0u].y * dropSize) drops[i].position.y = dropYRange(randomEngine);
			drops[i].model = scale(
				translate(identity<mat4>(), vec3{ drops[i].position.x, drops[i].position.y, drops[i].position.z }),
				vec3{ dropSize, dropSize, dropSize }
			);
			return Action::CONTINUE;
		});
	}
}