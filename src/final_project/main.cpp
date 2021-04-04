#include <iostream>
#include <chrono>

#include "ShadowViewerApp.hpp"
#include "ArgParser.hpp"

using namespace GLOO;
	
	int main(int argc, const char* argv[]) {
		ArgParser arg_parser(argc, argv);
		auto app = make_unique<ShadowViewerApp>("Assignment5", glm::ivec2(1440, 900));
		app->SetRenderingOptions(arg_parser);
		app->SetupScene();


		int i = 0;
		while (!app->IsFinished()) {
			i++;
			double delta_time = 1e-3f;//(current_tick_time - last_tick_time).count()*1e-6;
			double total_elapsed_time = i * 1e-3f;// (current_tick_time - start_tick_time).count()*1e-6;
			app->Tick(delta_time, total_elapsed_time);
			std::cout << total_elapsed_time << std::endl;
		}
		return 0;
	}

