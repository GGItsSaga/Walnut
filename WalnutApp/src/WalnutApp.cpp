#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include <iostream>
#include "Encrypt.h"
#include "Decrypt.h"


class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
	{
		e = std::make_unique<Encrypt>();
		d = std::make_unique<Decrypt>();
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Menu");

		if (ImGui::Button("Encrypt"))
		{
			encryptCallback();
		}

		if (ImGui::Button("Decrypt"))
		{
			decryptCallback();
		}

		ImGui::End();
	}

private:
	std::unique_ptr<Encrypt> e;
	std::unique_ptr<Decrypt> d;

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
