#include"to_back_end.h"
#pragma warning(disable:4996)
/*
��ʼ��MySQLDB
* 
*/
void initDB()
{
	if (!DBStatus::dealInitMySQLError(InitMySQL::initGlobleMySQL("127.0.0.1", "root", "hong456..", "tourism-recommend-sys-trafficimfor", 3306, "gbk")))
	{
		throw MyException(1,"INIT_MYSQL_DATABASE_FAILED","��ʼ�����ݿ����");
	}
}
/*
��ʼ��Redis
* 
*/
void initRedis()
{
	if (!dealInitRedisError(InitRedis::initRedis()))
	{
		throw MyException(1,"INIT_REDIS_FAILED","��ʼ��Redis����");
	}
}
/*
���UserRequirementFromCSharp�������Ƿ���ȷ
* 
*/
void requirementToVectorsOneGroup(const UserRequirementFromCSharp& requirement,
	vector<string>& start_cities, vector<string>& arrive_cities,
	vector<string>& start_time, vector<string>& arrive_time,
	vector<string>& vehicle_type, string& travel_type,
	vector<string>& transit_type, string& remark,
	vector<int>& distances)
{
	if (requirement.start_cities != nullptr && requirement.start_cities != "")
		start_cities = vector<string>(1, requirement.start_cities);
	else
		throw MyException(2, "EMPTY_START_CITIES_FROM_C#_ONE_GROUP", "C#�����˿յ�start_cities");

	if (requirement.arrive_cities != nullptr && requirement.arrive_cities != "")
		arrive_cities = vector<string>(1, requirement.arrive_cities);
	else
		throw MyException(2, "EMPTY_ARRIVAL_CITIES_FROM_C#_ONE_GROUP", "C#�����˿յ�arrive_cities");

	if (requirement.start_time != nullptr && requirement.start_time != "")
		start_time = vector<string>(1, requirement.start_time);
	else
		throw MyException(2, "EMPTY_START_TIME_FROM_C#_ONE_GROUP", "C#�����˿յ�start_time");

	if (requirement.arrive_time != nullptr && requirement.arrive_time != "")
		arrive_time = vector<string>(1, requirement.arrive_time);

	if (requirement.vehicle_type != nullptr && requirement.vehicle_type != "")
		vehicle_type = vector<string>(1, requirement.vehicle_type);
	else
		throw MyException(2, "EMPTY_VEHICLE_TYPE_FROM_C#_ONE_GROUP", "C#�����˿յ�vehicle_type");

	if (requirement.travel_type != nullptr && requirement.travel_type != "")
		travel_type = requirement.travel_type;
	else
		throw MyException(2, "EMPTY_TRAVEL_TYPE_FROM_C#_ONE_GROUP", "C#�����˿յ�travel_type");

	if (requirement.transit_type != nullptr && requirement.transit_type != "")
		transit_type = vector<string>(1, requirement.transit_type);
	else
		throw MyException(2, "EMPTY_TRAINSIT_TYPE_FROM_C#_ONE_GROUP", "C#�����˿յ�transit_type");

	if (requirement.remark != nullptr && requirement.remark != "")
		remark = requirement.remark;

	if (requirement.distances != 0)
		distances = vector<int>(1, requirement.distances);
	else
		throw MyException(2, "EMPTY_DISTANCE_FROM_C#_ONE_GROUP", "C#������Ϊ0��distances");
}

void GetRecommendations(UserRequirementFromCSharp requirement, RouteResult*& routes, char*& error,char*& remark,
	int& weight_size,int& first_route_size,int& second_route_size,int*& first_route_of_one_size_array, int*& second_route_of_one_size_array)
{
	try
	{
		/*UserRequirementAfterPretreat requirement_after = UserRequirement(requirement).pretreatUserRequirement();
		remark = (char*)requirement_after.remark.c_str();
		GetRoute get_route(requirement_after);
		get_route.getRouteResults(routes);
		weight_size = get_route.getWeights().size();
		for (int i = 0; i < weight_size; i++)
		{
			if (i == 0)
			{
				first_route_size = get_route.getWeights()[0].size();

				first_route_of_one_size_array = new int[first_route_size];
				for (int i = 0; i < first_route_size; i++)
				{
					first_route_of_one_size_array[i] = get_route.getWeights()[0][i].size();
				}
			}
			else if (i == 1)
			{
				second_route_size= get_route.getWeights()[1].size();
				second_route_of_one_size_array = new int[second_route_size];
				for (int i = 0; i < second_route_size; i++)
				{
					second_route_of_one_size_array[i] = get_route.getWeights()[1][i].size();
				}
			}
		}*/
	}
	catch (const std::exception& exc)
	{
		error = (char*)exc.what();
		return;
	}
}

void GetRecommendationsOneGroup(UserRequirementFromCSharp requirement, RouteResult*& routes, int& level, char*& error_code, char*& error, char*& remark,int& route_group_nums, int& route_size_in_routes, int*& first_route_of_one_size_array)
{
	try
	{
		initDB();
		initRedis();

		vector<string> start_cities;
		vector<string> arrive_cities;
		int city_num = 1;
		vector<string> start_time;
		vector<string> arrive_time;
		vector<string> vehicle_type;
		string travel_type;
		vector<string> transit_type;
		string remark;
		vector<int> distances;

		requirementToVectorsOneGroup(
			requirement,
			start_cities, arrive_cities,
			start_time, arrive_time,
			vehicle_type, travel_type,
			transit_type, remark, distances);

		UserRequirementAfterPretreat requirement_after = UserRequirement
		(
			start_cities,
			arrive_cities,
			city_num,
			start_time,
			arrive_time,
			vehicle_type,
			travel_type,
			transit_type,
			distances,
			remark
		).pretreatUserRequirement();
		remark = (char*)requirement_after.remark.c_str();
		GetRoute get_route(requirement_after);
		get_route.getRouteResultsOneGroup(routes, route_size_in_routes);

		route_group_nums = get_route.getWeights()[0].size();

		first_route_of_one_size_array = new int[route_group_nums];
		for (int i = 0; i < route_group_nums; i++)
		{
			first_route_of_one_size_array[i] = get_route.getWeights()[0][i].size();
		}

		level = 0;
		error_code = (char*)"NO_ERROR";
		error = (char*)"û�д���";
		return;
	}
	catch (const MyException& exc)
	{
		level = exc.getLevel();
		auto error_code_str = exc.getErrorCode();
		error_code = new char[error_code_str.length()];
		strcpy(error_code, error_code_str.c_str());
		string what_str = exc.what();
		error = new char[what_str.length()];
		strcpy(error, exc.what());
	}
	catch (const std::exception& exc)
	{
		level = 4;
		error_code = (char*)"OTHER_EXCEPTIONS";
		string what_str = exc.what();
		error = new char[what_str.length()];
		strcpy(error, exc.what());
		return;
	}
}

void FreeMemory(RouteResult*& routes, int weight_size, int first_route_size, int second_route_size)
{
	/*for (int i = 0; i < first_route_size; i++)
	{
		delete[] routes[0].route[i];
	}
	for (int i = 0; i < second_route_size; i++)
	{
		delete[] routes[1].route[i];
	}
	delete[] routes;*/
}

void FreeMemoryOneGroup(RouteResult*& routes,int*& first_route_of_one_size_array, int route_size_in_routes)
{
	for (int i = 0; i < route_size_in_routes; i++)
	{
		delete[] routes[i].route;
	}
	delete routes;
	delete first_route_of_one_size_array;
}