#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include <iostream>
#include <string>
#include "Encrypt.h"
#include "Decrypt.h"
#include "Account.h"


class ExampleLayer : public Walnut::Layer
{
private:
	Account account;
	bool isLoggedIn = false;
	bool showCreateAccountPopup = false;
	char usernameInput[128] = "";
	char passwordInput[128] = "";

public:
	ExampleLayer()
	{
		e = std::make_unique<Encrypt>();
		d = std::make_unique<Decrypt>();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Menu");

		if (!isLoggedIn)
		{
			ImGui::Text("Login");
			ImGui::InputText("Username", usernameInput, sizeof(usernameInput));
			ImGui::InputText("Password", passwordInput, sizeof(passwordInput), ImGuiInputTextFlags_Password);

			if (ImGui::Button("Log In"))
			{
				isLoggedIn = account.login(usernameInput, passwordInput);
				if (!isLoggedIn)
				{
					ImGui::OpenPopup("Login Failed");
				}
			}

			// Login failed popup:
			if (ImGui::BeginPopupModal("Login Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Login failed. Please try again.");
				if (ImGui::Button("OK"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}

		else // User has successfully logged in. Since thats the case, they are now able to encrypt and decrypt files.
		{
			// Calculate the horizontal position for center alignment:
			float windowWidth = ImGui::GetWindowWidth();
			float buttonWidth = 300.0f; // Width of the buttons
			float buttonHeight = 100.0f; // Height of the buttons
			float verticalPadding = (ImGui::GetWindowSize().y - buttonHeight * 2) / 3.0f; // Vertical padding

			// Add vertical space above the buttons
			ImGui::Spacing();
			ImGui::Dummy(ImVec2(0.0f, verticalPadding));

			// Calculate the horizontal position for center alignment
			float horizontalPadding = (windowWidth - buttonWidth * 2) / 2.0f;
			ImGui::SetCursorPosX(horizontalPadding);

			// Increase button text size
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

			if (ImGui::Button("Encrypt", ImVec2(buttonWidth, buttonHeight)))
			{
				encryptCallback();
			}

			ImGui::SameLine(); // Move to the same line for the next button

			if (ImGui::Button("Decrypt", ImVec2(buttonWidth, buttonHeight)))
			{
				decryptCallback();

			}

			ImGui::PopStyleVar(2);
		}

		ImGui::End();
	}

private:
	std::unique_ptr<Encrypt> e;
	std::unique_ptr<Decrypt> d;

	// Encryption call, will edit to where the user can enter what the file is named instead of hardcoding it.
	void encryptCallback()
	{
		try
		{
			// Input files:
			std::string inputFile1 = "Testing1.txt";
			std::string inputFile2 = "Testing2.txt";

			// Encrypt input files and save encrypted versions
			e->appendFile(inputFile1);
			e->appendFile(inputFile2);
			e->encryptAndSave();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}

	// Decryption call, will do the same to decryption call later.
	void decryptCallback()
	{
		d->setKey(e->getKey());

		std::vector<std::string> encryptedNames = e->getEncryptedFileNames();
		// try to display these in the GUI
		for (const auto& name : encryptedNames)
		{
			d->appendFile("Encrypted_" + name);
			d->appendFile("Encrypted_" + name);
		}
		d->decryptAndSaveAll();
	}
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Encryption API";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{
					app->Close();
				}
				ImGui::EndMenu();
			}
		});
	return app;
}
