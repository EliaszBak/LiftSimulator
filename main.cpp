// Dear ImGui: standalone example application for SDL2 + OpenGL
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <ctime>
#include <thread>
#include <mutex>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Passenger.hpp"
#include <stdio.h>
#include <SDL.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

#define GREY       IM_COL32(192, 192, 192, 255)
#define DARK_GREY  IM_COL32(128, 128, 128, 255)
#define RED        IM_COL32(255, 0, 0 ,255)
#define DARK_RED   IM_COL32(128, 0, 0 ,255)
#define GREEN      IM_COL32(0, 255, 0 ,255)
#define DARK_GREEN IM_COL32(0, 128, 0 ,255)
#define BLUE       IM_COL32(0, 0, 255 ,255)
#define DARK_BLUE  IM_COL32(0, 0, 128 ,255)

int seconds = 0 ;
bool runTimer = false;
std::mutex timer_mutex;
bool done = false;

void startCountdown()
{
    while (!done)
    {
        // bool runTimer_=false;
        // {
        //     std::lock_guard<std::mutex> g(timer_mutex);
        //     runTimer_ = runTimer;
        // }
        if (runTimer) 
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            seconds++;
        }
        else
            seconds = 0;
    }
}


// Main code
int main(int, char**)
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char* glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("Lift Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 1280, window_flags);
    SDL_SetWindowSize(window, 1280, 1280);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::push_backFont()/pop_frontFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Our state
    bool show_demo_window = false;
    bool show_another_window = false;
    bool show_elevator = true;
    const int passengerWeight = 70;
    const int maxCapacity = 600;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    std::deque<int> scheduledFloors;
    scheduledFloors.push_back(1);
    bool isEmpty = false;
    bool showError = false;
    ImVec2 currentPositionElevator = {547, 1235};                   // magic number heuheuheuhhuehueh
    int passengersInTheLift = 0;
    std::vector<Passenger> passengers_;
    std::thread t(startCountdown);

    // Main loop
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
            {
                done = true;
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            {
                done = true;
            }
        }
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;


            ImGui::Begin("Lift control room", NULL, 1 << 2);    
            ImVec2 pp = ImGui::GetCursorScreenPos();
            ImGui::Checkbox("display elevator", &show_elevator);
            bool b1 = ImGui::Button("1");  ImGui::SameLine();
            bool b2 = ImGui::Button("2");  ImGui::SameLine();
            bool b3 = ImGui::Button("3");  ImGui::SameLine();
            bool b4 = ImGui::Button("4");

            if (b1)                           
            {
                scheduledFloors.push_back(1);
                std::lock_guard<std::mutex> g(timer_mutex);
                runTimer = false;
                seconds = 0;
            }
            if (b2)   
            {
                scheduledFloors.push_back(2);
                std::lock_guard<std::mutex> g(timer_mutex);
                runTimer = false;
                seconds = 0;
            }                        
            if (b3)                           
            {
                scheduledFloors.push_back(3);
                std::lock_guard<std::mutex> g(timer_mutex);
                runTimer = false;
                seconds = 0;
            }
            if (b4)   
            {                         
                scheduledFloors.push_back(4);
                std::lock_guard<std::mutex> g(timer_mutex);
                runTimer = false;
                seconds = 0;
            }
            ImGui::SameLine();
            ImGui::Text("next floor = %d", scheduledFloors.front());


            static int in_ = 1;
            ImGui::InputInt("InFloor", &in_);
            static int out_ = 1;
            ImGui::InputInt("OutFloor", &out_);
            bool addP1 = ImGui::Button("Add passenger");


            if (addP1)
            {
                if (in_ != out_)
                    passengers_.push_back(Passenger(in_, out_));
            }


            ImGui::Text("ilosc zakolejkowanych = %d", int(scheduledFloors.size()) - 1);
            ImGui::Text("seconds = %d", seconds);
            ImGui::Text("Weight of the passengers = %d", passengersInTheLift*passengerWeight);


            pp.x +=300;
            pp.y -=30;
            ImGui::SetNextWindowPos(pp);
            ImGui::SetNextWindowSize({800,1200});
            if (show_elevator)
            {
                ImGui::Begin("elevator visualizaton", &show_elevator); 
                ImVec2 startingPoint = ImGui::GetCursorScreenPos();
                ImDrawList* draw_list = ImGui::GetWindowDrawList();
                ImU32 frame_color = ImGui::GetColorU32(IM_COL32_WHITE);
                ImU32 floorcolor = ImGui::GetColorU32(GREY);
                ImU32 elevator_color = ImGui::GetColorU32(DARK_GREY);
                ImU32 passenger_color = ImGui::GetColorU32(GREEN);
                const float elevatorHeight = 1180.0f;
                const float elevatorWidth = 780.0f;
                const float thickness = 3.0f;
                const float secondFloorHeight = elevatorHeight-elevatorHeight/4;
                const float thirdFloorHeight = elevatorHeight/2;
                const float fourthFloorHeight = elevatorHeight/4;
                ImVec2 positionOfElevator1 = {startingPoint.x+200, startingPoint.y+elevatorHeight};
                ImVec2 positionOfElevator2 = {startingPoint.x+200, startingPoint.y+secondFloorHeight-55};
                ImVec2 positionOfElevator3 = {startingPoint.x+200, startingPoint.y+thirdFloorHeight-55};
                ImVec2 positionOfElevator4 = {startingPoint.x+200, startingPoint.y+fourthFloorHeight-55};
                // elevator frame
                draw_list->AddLine(ImVec2(startingPoint.x, startingPoint.y), ImVec2(startingPoint.x, startingPoint.y+ elevatorHeight), frame_color, thickness); 
                draw_list->AddLine(ImVec2(startingPoint.x+elevatorWidth, startingPoint.y), ImVec2(startingPoint.x + elevatorWidth, startingPoint.y + elevatorHeight), frame_color, thickness); 
                draw_list->AddLine(ImVec2(startingPoint.x, startingPoint.y), ImVec2(startingPoint.x + elevatorWidth, startingPoint.y ), frame_color, thickness); 

                // floors
                draw_list->AddLine(ImVec2(startingPoint.x, fourthFloorHeight), ImVec2(startingPoint.x+elevatorWidth, fourthFloorHeight), floorcolor, thickness);
                draw_list->AddLine(ImVec2(startingPoint.x, thirdFloorHeight), ImVec2(startingPoint.x+elevatorWidth, thirdFloorHeight), floorcolor, thickness);
                draw_list->AddLine(ImVec2(startingPoint.x, secondFloorHeight), ImVec2(startingPoint.x+elevatorWidth, secondFloorHeight), floorcolor, thickness);

                if (scheduledFloors.front() == 1)
                {
                    if (currentPositionElevator.y < positionOfElevator1.y)
                    {
                        currentPositionElevator.y+=1;
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else
                    {
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);    
                        if (scheduledFloors.size()>1)
                            scheduledFloors.pop_front();
                        for (auto iter = passengers_.begin(); iter!=passengers_.end();)
                        {
                            if (iter->floorIn == 1 && iter->isIn == false)
                            {
                                iter->isIn = true;
                                passengersInTheLift++;
                                iter++;
                            }
                            if (iter->floorOut == 1 && iter->isIn == true)
                            {
                                iter->isIn = false;
                                iter = passengers_.erase(iter);
                                passengersInTheLift--;
                            }
                        }
                        std::lock_guard<std::mutex> g(timer_mutex);
                        runTimer = false;
                        seconds = 0;
                    }
                }
                if (scheduledFloors.front() == 2)
                {
                    if (currentPositionElevator.y > positionOfElevator2.y)
                    {
                        currentPositionElevator.y-=1; 
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else if (currentPositionElevator.y < positionOfElevator2.y)
                    {
                        currentPositionElevator.y+=1;
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else
                    {
                        draw_list->AddRectFilled(currentPositionElevator,
                            ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);    
                        if (scheduledFloors.size()>1)
                            scheduledFloors.pop_front();
                        for (auto iter = passengers_.begin(); iter!=passengers_.end();)
                        {
                            if (iter->floorIn == 2 && iter->isIn == false)
                            {
                                iter->isIn = true;
                                passengersInTheLift++;
                                iter++;
                            }
                            if (iter->floorOut == 2 && iter->isIn == true)
                            {
                                iter->isIn = false;
                                iter = passengers_.erase(iter);
                                passengersInTheLift--;
                            }
                        }
                        if (scheduledFloors.size()<=1  && isEmpty && scheduledFloors.front()==2)
                        {
                            std::lock_guard<std::mutex> g(timer_mutex);
                            runTimer = true;
                        }
                        if (seconds >=5)
                        {
                            scheduledFloors.push_back(1);
                        }
                    }
                }
                if (scheduledFloors.front() == 3)
                {
                    if (currentPositionElevator.y > positionOfElevator3.y)
                    {
                        currentPositionElevator.y-=1; 
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
    
                    }
                    else if (currentPositionElevator.y < positionOfElevator3.y)
                    {
                        currentPositionElevator.y+=1;
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else
                    {
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);    
                        if (scheduledFloors.size()>1)
                            scheduledFloors.pop_front();
                        for (auto iter = passengers_.begin(); iter!=passengers_.end();)
                        {
                            if (iter->floorIn == 3 && iter->isIn == false)
                            {
                                iter->isIn = true;
                                passengersInTheLift++;
                                iter++;
                            }
                            if (iter->floorOut == 3 && iter->isIn == true)
                            {
                                iter->isIn = false;
                                iter = passengers_.erase(iter);
                                passengersInTheLift--;
                            }
                        }
                        if (scheduledFloors.size()<=1 && isEmpty && scheduledFloors.front()==3)
                        {
                            std::lock_guard<std::mutex> g(timer_mutex);
                            runTimer = true;
                        }
                        if (seconds >=5)
                        {
                            scheduledFloors.push_back(1);
                        }
                    } 
                }
                if (scheduledFloors.front() == 4)
                {
                    if (currentPositionElevator.y > positionOfElevator4.y)
                    {
                        currentPositionElevator.y-=1; 
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else if (currentPositionElevator.y < positionOfElevator4.y)
                    {
                        currentPositionElevator.y+=1;
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);
                    }
                    else
                    {
                        draw_list->AddRectFilled(currentPositionElevator, ImVec2(currentPositionElevator.x + 400, currentPositionElevator.y - 250), elevator_color);    
                        if (scheduledFloors.size()>1)
                            scheduledFloors.pop_front();
                        for (auto iter = passengers_.begin(); iter!=passengers_.end();)
                        {
                            if (iter->floorIn == 4 && iter->isIn == false)
                            {
                                iter->isIn = true;
                                passengersInTheLift++;
                                iter++;
                            }
                            if (iter->floorOut == 4 && iter->isIn == true)
                            {
                                iter->isIn = false;
                                iter = passengers_.erase(iter);
                                passengersInTheLift--;
                            }
                        }
                        if (scheduledFloors.size()<=1 && isEmpty && scheduledFloors.front()==4)
                        {
                            std::lock_guard<std::mutex> g(timer_mutex);
                            runTimer = true;
                        }
                        if (seconds >=5)
                        {
                            scheduledFloors.push_back(1);
                        }
                    } 
                }

                // passangers
                int spacing = 10;
                for (const auto& passanger: passengers_)
                {
                    
                    if (!(std::find(scheduledFloors.begin(), scheduledFloors.end(), passanger.floorOut) != scheduledFloors.end()))
                    {
                        if (passanger.isIn)
                        {
                            scheduledFloors.push_back(passanger.floorOut);
                            
                            runTimer = false;
                        }
                    }
                    if (passanger.isIn)
                    {
                        draw_list->AddRectFilled(ImVec2(currentPositionElevator.x + spacing, currentPositionElevator.y),
                            ImVec2(currentPositionElevator.x + spacing + 50, currentPositionElevator.y - 50), passenger_color);
                        spacing+=60;
                    }
                }

                auto is_in = [](Passenger x) { return x.isIn;};


                if (passengersInTheLift ==0)
                    isEmpty = true;
                else
                    isEmpty = false;

            //     if (std::find_if(passengers_.begin(), passengers_.end(), is_in) !=passengers_.end())
            //         isEmpty = false;
            //     else
            //         isEmpty = true;
                ImGui::End();
            }
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }
    t.join();
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
