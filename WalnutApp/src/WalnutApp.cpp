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
	char usernameInput[128] = "", passwordInput[128] = ""; // Username and password entry
	char newUsername[128] = "", newPassword[128] = ""; // New username and password entry
	std::string errorMessage, accountCreationResult; // Error message output for logins and account creation check
	char inputFileEncrypt[128] = ""; // Encrypt file input
	char inputFileDecrypt[128] = ""; // Decrypt file input
	std::string encryptError, decryptError; // Encrypt and decrypt errors

public:
	ExampleLayer()
	{
		e = std::make_unique<Encrypt>();
		d = std::make_unique<Decrypt>();
	}

	// Render method for GUI
	virtual void OnUIRender() override
	{
		ImGui::Begin("Menu");

		// If user is not logged in, prompt them towards the login screen. 
		// This will ask the user to input their username and password from the accounts.txt file. If it's correct, login. If not, prompt them an error.
		// If a user is accessing this for the first time, they can create an account. If the username is already registered then it will output an error that
		// tells the user that it's already taken, and then re-prompt them to enter their information. If successful, writes that information to accounts.txt and exits.
		// After exiting account creation, users will use that login and password that they just made in order to access the system. If user login is successful...
		if (!isLoggedIn)
		{
			ImGui::Text("Login"); 
			ImGui::InputText("Username", usernameInput, sizeof(usernameInput));
			ImGui::InputText("Password", passwordInput, sizeof(passwordInput), ImGuiInputTextFlags_Password);

			if (ImGui::Button("Log In"))
			{
				// Checks to see if the user input is a valid input. If it is, skip. If not, print popup.
				isLoggedIn = account.login(usernameInput, passwordInput);
				if (!isLoggedIn)
				{
					ImGui::OpenPopup("Login Failed"); // Popup call
				}
			}

			// Button to open log file on login screen
			if (ImGui::Button("Access Public Log"))
			{
				std::string logFileName = "public log.txt";

				// Checks to see if log exists.
				std::ifstream logFile(logFileName);
				if (logFile.good())
				{
					logFile.close();
					std::string command = "start notepad " + logFileName;
					std::system(command.c_str());
				}
				else
				{
					ImGui::OpenPopup("Log Not Found");
				}
			}

			// If log isn't found, run this popup window. 
			if (ImGui::BeginPopupModal("Log Not Found", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("No public log file found.");
				if (ImGui::Button("OK"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Login failed popup:
			if (ImGui::BeginPopupModal("Login Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Username or Password incorrect. Please re-enter.");
				if (ImGui::Button("OK"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			// Create Account popup:
			if (ImGui::Button("Create Account"))
			{
				showCreateAccountPopup = true;
			}

			if (showCreateAccountPopup)
			{
				ImGui::OpenPopup("Create New Account");
				if (ImGui::BeginPopupModal("Create New Account", &showCreateAccountPopup, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::InputText("New Username", newUsername, sizeof(newUsername)); // User enters new username
					ImGui::InputText("New Password", newPassword, sizeof(newPassword), ImGuiInputTextFlags_Password); // User enters new password
					if (ImGui::Button("Register"))
					{
						accountCreationResult = account.createAccount(newUsername, newPassword);
						if (accountCreationResult == "Account created successfully")
						{
							showCreateAccountPopup = false;
							ImGui::CloseCurrentPopup();
						}
						else if (accountCreationResult == "Username taken")
						{
							ImGui::OpenPopup("Username Taken");
						}
					}

					// Error popup for username taken
					if (ImGui::BeginPopupModal("Username Taken", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Error: Username is already taken.");
						if (ImGui::Button("OK"))
						{
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					// General error if there is any for creating an account: 
					if (ImGui::BeginPopupModal("Create Account Error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::Text("Error creating account:");
						ImGui::TextWrapped(errorMessage.c_str());
						if (ImGui::Button("OK"))
						{
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel"))
					{
						showCreateAccountPopup = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}
		}

		else // User has successfully logged in. Since thats the case, they are now able to encrypt and decrypt files.
		{
			// Center allignment calculations for Encrypt and Decrypt buttons. The next three sectons up until the Encrypt and Decrypt button call is general UI formatting.
			float windowWidth = ImGui::GetWindowWidth();
			float buttonWidth = 300.0f; // Width of the buttons
			float buttonHeight = 100.0f; // Height of the buttons
			float verticalPadding = (ImGui::GetWindowSize().y - buttonHeight * 2) / 3.0f; // Vertical padding

			// Add vertical space above the buttons
			ImGui::Spacing();
			ImGui::Dummy(ImVec2(0.0f, verticalPadding));

			// Calculate the horizontal position for center alignment:
			float horizontalPadding = (windowWidth - buttonWidth * 2) / 2.0f;
			ImGui::SetCursorPosX(horizontalPadding);

			// Increase button text size:
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20, 20));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(20, 20));

			// Encrypt button 
			if (ImGui::Button("Encrypt", ImVec2(buttonWidth, buttonHeight)))
			{
				ImGui::OpenPopup("Encrypt Files");
			}

			ImGui::SameLine(); // Move to the same line for the next button

			// Decrypt button
			if (ImGui::Button("Decrypt", ImVec2(buttonWidth, buttonHeight)))
			{
				ImGui::OpenPopup("Decrypt Files");
			}

			ImGui::PopStyleVar(2);

			// Encrypt popup. When "Encrypt" is pressed, a pop-up will come up, and will prompt user to enter the file they wish to encrypt.
			// When they put in the file name, it calls the encryptCallBack method.
			// If the file doesn't exist, outputs an error saying that the file does not exist.
			if (ImGui::BeginPopupModal("Encrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::InputText("File to Encrypt", inputFileEncrypt, sizeof(inputFileEncrypt));

				if (ImGui::Button("Encrypt"))
				{
					if (fileExists(inputFileEncrypt))
					{
						encryptCallback(inputFileEncrypt);
						ImGui::CloseCurrentPopup();
					}
					else
					{
						encryptError = "Error: File not found.";
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				if (!encryptError.empty())
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), encryptError.c_str());
				}

				ImGui::EndPopup();
			}

			// Encrypt popup. When "Decrypt" is pressed, a pop-up will come up, and will prompt user to enter the file they wish to decrypt.
			// When they put in the file name, it calls the decryptCallBack method.
			// If the file doesn't exist, outputs an error saying that the file does not exist.
			if (ImGui::BeginPopupModal("Decrypt Files", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::InputText("File to Decrypt", inputFileDecrypt, sizeof(inputFileDecrypt));

				if (ImGui::Button("Decrypt"))
				{
					if (fileExists(inputFileDecrypt))
					{
						decryptCallback(inputFileDecrypt);
						ImGui::CloseCurrentPopup();
					}
					else
					{
						decryptError = "Error: File not found.";
					}
				}

				ImGui::SameLine();

				if (ImGui::Button("Cancel"))
				{
					ImGui::CloseCurrentPopup();
				}

				if (!decryptError.empty())
				{
					ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), decryptError.c_str());
				}

				ImGui::EndPopup();
			}

			// Encryption log: 
			// Center align the "Open Log File" button. Why do this? 
			// Makes GUI match what we had originally while making the program look cleaner as a result.
			float logButtonPosX = (windowWidth - buttonWidth) / 2.0f;
			ImGui::SetCursorPosX(logButtonPosX);
			
			// Encryption/Decryption log opener:
			if (ImGui::Button("Open Log File", ImVec2(buttonWidth, buttonHeight)))
			{
				std::string logFileName = "public log.txt";

				// Checks to see if log exists. If it exists, pushes a command that opens notepad in windows. 
				// Can be implemented into Mac/Linux but for this particular case, we use windows commands.
				std::ifstream logFile(logFileName);
				if (logFile.good())
				{
					logFile.close();
					std::string command = "start notepad " + logFileName;
					std::system(command.c_str());
				}
				else
				{
					ImGui::OpenPopup("Log File Not Found");
				}
			}

			// If log isn't found, run this popup window. 
			if (ImGui::BeginPopupModal("Log File Not Found", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Error: No public log file found.");
				if (ImGui::Button("OK"))
				{
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}

private:
	std::unique_ptr<Encrypt> e;
	std::unique_ptr<Decrypt> d;

	// Encryption call:
	void encryptCallback(const char* inputFile)
	{
		try
		{
			// Logs encryption:
			std::ofstream logFile("public log.txt", std::ios_base::app);
			if (logFile.is_open())
			{
				logFile << "File Encrypted: " << inputFile << std::endl;
				logFile << "User: " << account.getUsername() << std::endl;
				logFile << "Time: " << getCurrentDateTime() << std::endl;
				logFile << std::endl;
				logFile.close();
			}
			else
			{
				std::cerr << "Error opening log file for writing." << std::endl;
			}

			// Appends and encrypts file typed in by the user.
			e->appendFile(inputFile);
			e->encryptAndSave();
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}
	}

	// Decryption call: 
	void decryptCallback(const char* inputFile)
	{
		try
		{
			// Logs decryption:
			std::ofstream logFile("public log.txt", std::ios_base::app);
			if (logFile.is_open())
			{
				logFile << "File Decrypted: " << inputFile << std::endl;
				logFile << "User: " << account.getUsername() << std::endl;
				logFile << "Time: " << getCurrentDateTime() << std::endl;
				logFile << std::endl;
				logFile.close();
			}
			else
			{
				std::cerr << "Error opening log file for writing." << std::endl;
			}

			// Gets decryption key
			d->setKey(e->getKey());

			// Appends file and decrypt the encrypted file
			d->appendFile(inputFile);
			d->decryptAndSaveAll(); // Assuming this method decrypts and saves the decrypted content
		}
		catch (const std::exception& e)
		{
			std::cerr << "Error during decryption: " << e.what() << std::endl;
			// Handle decryption error if needed
		}
	}

	// Checks if a file exits or not for encryption or decryption.
	bool fileExists(const char* fileName)
	{
		std::ifstream file(fileName);
		return file.good();
	}

	// Gets the current date and time for public log:
	std::string getCurrentDateTime()
	{
		std::time_t now = std::time(nullptr);
		char buffer[80];
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
		return std::string(buffer);
	}

};

// Main method for application call:
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
