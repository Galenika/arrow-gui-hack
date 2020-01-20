#include "backtrack.h"
#include "../features.hpp"
#include "../../menu/fgui_menu.hpp"
#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (float)(dt) / interfaces::globals->interval_per_tick ) )
vec3_t GetHitboxPositionv69(player_t* obj, int Hitbox)
{
	matrix_t bone_matrix[128];
	if (obj->setup_bones(bone_matrix, 128, 0x00000100, obj->simulation_time())) {
		if (obj->model())
		{
			auto studio_model = interfaces::model_info->get_studio_model(obj->model());
			if (studio_model)
			{
				auto hitbox = studio_model->hitbox_set(0)->hitbox(Hitbox);
				vec3_t Point[] =
				{
					vec3_t(hitbox->mins.x, hitbox->mins.y, hitbox->mins.z),
					vec3_t(hitbox->mins.x, hitbox->maxs.y, hitbox->mins.z),
					vec3_t(hitbox->maxs.x, hitbox->maxs.y, hitbox->mins.z),
					vec3_t(hitbox->maxs.x, hitbox->mins.y, hitbox->mins.z),
					vec3_t(hitbox->maxs.x, hitbox->maxs.y, hitbox->maxs.z),
					vec3_t(hitbox->mins.x, hitbox->maxs.y, hitbox->maxs.z),
					vec3_t(hitbox->mins.x, hitbox->mins.y, hitbox->maxs.z),
					vec3_t(hitbox->maxs.x, hitbox->mins.y, hitbox->maxs.z)
				};
				vec3_t vMin, vMax, vCenter, sCenter;

				int head = 100;
				int body = 100;
				int legs = 100;

				math::transform_vector(hitbox->mins, bone_matrix[hitbox->bone], vMin);
				math::transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], vMax);

				vCenter = ((vMin + vMax) * 0.5f);
				int iCount = 7;

				for (int i = 0; i <= iCount; i++)
				{
					vec3_t vTargetPos;
					switch (i)
					{
					case 0:
					default:
						vTargetPos = vCenter; break;

					case 1:
					{
						vTargetPos = (Point[7] + Point[1]) * (head / 100);
					}
					break;

					case 2:
					{
						vTargetPos = (Point[3] + Point[4]) * (body / 100);
					}
					break;

					case 3:
					{
						vTargetPos = (Point[4] + Point[0]) * (body / 100);
					}
					break;

					case 4:
					{
						vTargetPos = (Point[2] + Point[7]) * (35 / 100);
					}
					break;

					case 5:
					{
						vTargetPos = (Point[4] + Point[0]) * (legs / 100);
					}
					break;
					case 6:
						vTargetPos = (Point[5] + Point[3]) * 0.6;
						break;
					case 7:
						vTargetPos = (Point[1] + Point[2]) * 0.6;
						break;
						//	default: vTargetPos = vCenter;
					}
					return vTargetPos;
				}

			}

		}
	}
	return vec3_t{};
}
void BackTrack::Update(int tick_count)
{
	//if (!menu.config.fakelag_check)
	//	return;

	latest_tick = tick_count;
	for (int i = 0; i < 64; i++)
	{
		UpdateRecord(i);
	}
}

bool BackTrack::IsTickValid(int tick)
{
	int delta = latest_tick - tick;
	float deltaTime = delta * interfaces::globals->interval_per_tick;
	return (fabs(deltaTime) <= 0.2f);
}
void BackTrack::UpdateRecord(int i)
{
	//C_BaseEntity* pEntity = g_pEntityList->GetClientEntity(i);
	//if (pEntity && pEntity->IsAlive() && !pEntity->IsDormant())
	//{
	//	float lby = pEntity->GetLowerBodyYaw();
	//	if (lby != records[i].lby)
	//	{
	//		records[i].tick_count = latest_tick;
	//		records[i].lby = lby;
	//		records[i].headPosition = GetHitboxPositionv69(pEntity, 0);
	//	}
	//}
	//else
	//{
	//	records[i].tick_count = 0;
	//}
}

bool BackTrack::RunLBYBackTrack(int i, c_usercmd* cmd, vec3_t& aimPoint)
{
	if (IsTickValid(records[i].tick_count))
	{
		aimPoint = records[i].headPosition;
		cmd->tick_count = records[i].tick_count;
		return true;
	}
	return false;
}

vec3_t angle_vector(vec3_t meme)
{
	auto sy = sin(meme.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(meme.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(meme.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(meme.x / 180.f * static_cast<float>(M_PI));

	return vec3_t(cp * cy, cp * sy, -sp);
}

float distance_point_to_line(vec3_t Point, vec3_t LineOrigin, vec3_t Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).length();
}

void BackTrack::legitBackTrack(c_usercmd* cmd, player_t* pLocal)
{
	if (cfg.backtrack == 1)
	{
		int bestTargetIndex = -1;
		float bestFov = FLT_MAX;
		player_info_t info;
		if (!pLocal->is_alive())
			return;

		for (int i = 0; i < interfaces::globals->max_clients; i++)
		{
			auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

			if (!entity || !pLocal)
				continue;

			if (entity == pLocal)
				continue;

			interfaces::engine->get_player_info(i, &info);

			if (entity->dormant())
				continue;

			if (entity->team() == pLocal->team())
				continue;

			if (entity->is_alive())
			{

				float simtime = entity->simulation_time();
				vec3_t hitboxPos = GetHitboxPositionv69(entity, 0);

				headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
				vec3_t ViewDir = angle_vector(cmd->viewangles + (pLocal->aim_punch_angle() * 2.f));
				float FOVDistance = distance_point_to_line(hitboxPos, pLocal->get_eye_pos(), ViewDir);

				if (bestFov > FOVDistance)
				{
					bestFov = FOVDistance;
					bestTargetIndex = i;
				}
			}
		}

		float bestTargetSimTime;
		if (bestTargetIndex != -1)
		{
			float tempFloat = FLT_MAX;
			vec3_t ViewDir = angle_vector(cmd->viewangles + (pLocal->aim_punch_angle() * 2.f));
			for (int t = 0; t < 12; ++t)
			{
				float tempFOVDistance = distance_point_to_line(headPositions[bestTargetIndex][t].hitboxPos, pLocal->get_eye_pos(), ViewDir);
				if (tempFloat > tempFOVDistance&& headPositions[bestTargetIndex][t].simtime > pLocal->simulation_time() - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
				}
			}
			if (cmd->buttons & in_attack)
			{
				cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
			}
		}
	}
}
BackTrack* backtracking = new BackTrack();
backtrackData headPositions[64][12];


//template<class T, class U>
//inline T clamp(T in, U low, U high)
//{
//	if (in <= low)
//		return low;
//	else if (in >= high)
//		return high;
//	else
//		return in;
//}
//c_backtrack backtrack;
//
//std::deque<stored_records> records[65];
//convars cvars;
//
//float c_backtrack::get_lerp_time() noexcept {
//	auto ratio = clamp(cvars.interp_ratio->float_value, cvars.min_interp_ratio->float_value, cvars.max_interp_ratio->float_value);
//	return max(cvars.interp->float_value, (ratio / ((cvars.max_update_rate) ? cvars.max_update_rate->float_value : cvars.update_rate->float_value)));
//}
//
//int c_backtrack::time_to_ticks(float time) noexcept {
//	return static_cast<int>((0.5f + static_cast<float>(time) / interfaces::globals->interval_per_tick));
//}
//
//bool c_backtrack::valid_tick(float simtime) noexcept {
//	auto network = interfaces::engine->get_net_channel_info();
//	if (!network)
//		return false;
//
//	auto delta = clamp(network->get_latency(0) + get_lerp_time(), 0.f, cvars.max_unlag->float_value) - (interfaces::globals->cur_time - simtime);
//	return std::fabsf(delta) <= 0.2f;
//}
//
//void c_backtrack::update() noexcept {
//	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
//	if (!menu.config.enable_legit_bt || !local_player || !local_player->is_alive()) {
//		if (!records->empty())
//			records->clear();
//
//		return;
//	}
//
//	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
//		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
//		if (!entity || entity == local_player || entity->dormant() || !entity->is_alive() || !entity->is_enemy()) {
//			records[i].clear();
//			continue;
//		}
//
//		if (records[i].size() && (records[i].front().simulation_time == entity->simulation_time()))
//			continue;
//
//		auto var_map = reinterpret_cast<uintptr_t>(entity) + 0x24;
//		auto vars_count = *reinterpret_cast<int*>(static_cast<uintptr_t>(var_map) + 0x14);
//		for (int j = 0; j < vars_count; j++)
//			*reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(var_map) + j * 0xC) = 0;
//
//		stored_records record{ };
//		record.head = local_player->get_hitbox_position(entity, hitbox_head);
//		record.simulation_time = entity->simulation_time();
//
//		entity->setup_bones(record.matrix, 128, 0x7FF00, interfaces::globals->cur_time);
//
//		records[i].push_front(record);
//
//		while (records[i].size() > 3 && records[i].size() > static_cast<size_t>(time_to_ticks(static_cast<float>(200.f) / 1000.f)))
//			records[i].pop_back();
//
//		if (auto invalid = std::find_if(std::cbegin(records[i]), std::cend(records[i]), [](const stored_records& rec) { return !backtrack.valid_tick(rec.simulation_time); }); invalid != std::cend(records[i]))
//			records[i].erase(invalid, std::cend(records[i]));
//	}
//}
//
//void c_backtrack::run(c_usercmd* cmd) noexcept {
//	if (!menu.config.enable_legit_bt)
//		return;
//
//	if (!(cmd->buttons & in_attack))
//		return;
//
//	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
//	if (!local_player)
//		return;
//
//	auto best_fov{ 255.f };
//	player_t* best_target{ };
//	int besst_target_index{ };
//	vec3_t best_target_head_position{ };
//	int best_record{ };
//
//	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
//		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
//		if (!entity || entity == local_player || entity->dormant() || !entity->is_alive()
//			|| !entity->is_enemy())
//			continue;
//
//		auto head_position = entity->get_bone_position(8);
//
//		auto angle = math::calculate_angle2(local_player->get_eye_pos(), head_position, cmd->viewangles);
//		auto fov = std::hypotf(angle.x, angle.y);
//		if (fov < best_fov) {
//			best_fov = fov;
//			best_target = entity;
//			besst_target_index = i;
//			best_target_head_position = head_position;
//		}
//	}
//
//	if (best_target) {
//		if (records[besst_target_index].size() <= 3)
//			return;
//
//		best_fov = 255.f;
//
//		for (size_t i = 0; i < records[besst_target_index].size(); i++) {
//			auto record = &records[besst_target_index][i];
//			if (!record || !valid_tick(record->simulation_time))
//				continue;
//
//			auto angle = math::calculate_angle2(local_player->get_eye_pos(), record->head, cmd->viewangles);
//			auto fov = std::hypotf(angle.x, angle.y);
//			if (fov < best_fov) {
//				best_fov = fov;
//				best_record = i;
//			}
//		}
//	}
//
//	if (best_record) {
//		auto record = records[besst_target_index][best_record];
//		cmd->tick_count = time_to_ticks(record.simulation_time);
//	}
//}