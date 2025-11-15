#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>

// Color definitions
const std::string COLOR_CYAN = "\033[38;2;0;255;255m";
const std::string COLOR_RED = "\033[31m";
const std::string COLOR_GREEN = "\033[32m";
const std::string COLOR_YELLOW = "\033[33m";
const std::string COLOR_ORANGE = "\033[38;5;208m";
const std::string COLOR_RESET = "\033[0m";

class SpitfireFolderInstaller {
private:
    std::string target_folder;
    std::string new_username;
    std::string root_password;
    std::string user_password;

    void display_header() {
        std::cout << COLOR_ORANGE;
        std::cout << "SPITFIRE CKGE MINIMAL - FOLDER INSTALL" << std::endl;
        std::cout << "======================================" << std::endl;
        std::cout << COLOR_RESET << std::endl;
    }

    int execute_command(const std::string& cmd) {
        std::cout << COLOR_CYAN << "[EXEC] " << cmd << COLOR_RESET << std::endl;
        int status = system(cmd.c_str());
        if (status != 0) {
            std::cerr << COLOR_RED << "Error: " << cmd << COLOR_RESET << std::endl;
        }
        return status;
    }

    std::string get_input(const std::string& prompt) {
        std::cout << COLOR_CYAN << prompt << COLOR_RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    bool create_directory(const std::string& path) {
        return system(("mkdir -p " + path).c_str()) == 0;
    }

public:
    void run() {
        display_header();

        // Get user input
        target_folder = get_input("Enter target folder path: ");
        new_username = get_input("Enter username: ");
        root_password = get_input("Enter root password: ");
        user_password = get_input("Enter user password: ");

        std::cout << COLOR_CYAN << "Starting Spitfire CKGE Minimal installation in: " << target_folder << COLOR_RESET << std::endl;

        // YOUR EXACT COMMANDS - ADAPTED FOR FOLDER
        std::cout << COLOR_CYAN << "Setting up base structure..." << COLOR_RESET << std::endl;
        create_directory(target_folder);
        create_directory(target_folder + "/boot");
        create_directory(target_folder + "/boot/grub");
        create_directory(target_folder + "/etc");
        create_directory(target_folder + "/usr/share/grub/themes");
        create_directory(target_folder + "/usr/share/plymouth/themes");
        create_directory(target_folder + "/usr/local/bin");
        create_directory(target_folder + "/etc/systemd/system");
        create_directory(target_folder + "/etc/sddm.conf.d");
        create_directory(target_folder + "/home/" + new_username + "/.config");
        create_directory(target_folder + "/home/" + new_username + "/.local/share/konsole");
        create_directory(target_folder + "/opt");

        // YOUR EXACT COMMANDS
        execute_command("cp -r /opt/claudemods-distribution-installer/vconsole.conf " + target_folder + "/etc");

        execute_command("cp -r /etc/resolv.conf " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d " + target_folder + "/etc");
        execute_command("unzip -o /opt/claudemods-distribution-installer/pacman.d.zip -d /etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf " + target_folder + "/etc");
        execute_command("cp -r /opt/claudemods-distribution-installer/pacman.conf /etc");

        execute_command("pacman -Sy");
        execute_command("pacstrap " + target_folder + " claudemods-desktop");
        execute_command("mkdir -p " + target_folder + "/boot");
        execute_command("mkdir -p " + target_folder + "/boot/grub");
        execute_command("touch " + target_folder + "/boot/grub/grub.cfg.new");

        execute_command("mount --bind /dev " + target_folder + "/dev");
        execute_command("mount --bind /dev/pts " + target_folder + "/dev/pts");
        execute_command("mount --bind /proc " + target_folder + "/proc");
        execute_command("mount --bind /sys " + target_folder + "/sys");
        execute_command("mount --bind /run " + target_folder + "/run");

        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable sddm\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable NetworkManager\"");

        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub " + target_folder + "/etc/default");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/grub.cfg " + target_folder + "/boot/grub");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos " + target_folder + "/usr/share/grub/themes");
        execute_command("chroot " + target_folder + " /bin/bash -c \"grub-mkconfig -o /boot/grub/grub.cfg\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/cachyos-bootanimation " + target_folder + "/usr/share/plymouth/themes/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.sh " + target_folder + "/usr/local/bin");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/local/bin/term.sh\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/term.service " + target_folder + "/etc/systemd/system/");
        execute_command("chroot " + target_folder + " /bin/bash -c \"systemctl enable term.service >/dev/null 2>&1\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"plymouth-set-default-theme -R cachyos-bootanimation\"");

        execute_command("chroot " + target_folder + " /bin/bash -c \"useradd -m -G wheel -s /bin/bash " + new_username + "\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo 'root:" + root_password + "' | chpasswd\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo '" + new_username + ":" + user_password + "' | chpasswd\"");
        execute_command("chroot " + target_folder + " /bin/bash -c \"echo '%wheel ALL=(ALL:ALL) ALL' | tee -a /etc/sudoers\"");

        execute_command("chmod +x " + target_folder + "/home/" + new_username + "/.config/fish/config.fish");
        execute_command("chroot " + target_folder + " /bin/bash -c \"chmod +x /usr/share/fish/config.fish\"");

        execute_command("cd " + target_folder);
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/claudemods-desktop/spitfire-minimal.zip");
        execute_command("wget --show-progress --no-check-certificate --continue --tries=10 --timeout=30 --waitretry=5 https://claudemodsreloaded.co.uk/arch-systemtool/Arch-Systemtool.zip");
        execute_command("unzip -o " + target_folder + "/Arch-Systemtool.zip -d " + target_folder + "/opt");
        execute_command("unzip -o " + target_folder + "/spitfire-minimal.zip -d " + target_folder + "/home/" + new_username + "/");
        execute_command("mkdir -p " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/kde_settings.conf " + target_folder + "/etc/sddm.conf.d");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/tweaksspitfire.sh " + target_folder + "/opt");
        execute_command("chmod +x " + target_folder + "/opt/tweaksspitfire.sh");
        execute_command("chroot " + target_folder + " /bin/bash -c \"su - " + new_username + " -c 'cd /opt && ./tweaksspitfire.sh " + new_username + "'\"");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/konsolerc " + target_folder + "/home/" + new_username + "/.config/");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/SpitFireLogin " + target_folder + "/usr/share/sddm/themes");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.colorscheme " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("cp -r /opt/claudemods-distribution-installer/spitfire-ckge-minimal/claudemods-cyan.profile " + target_folder + "/home/" + new_username + "/.local/share/konsole");
        execute_command("rm -rf " + target_folder + "/Arch-Systemtool.zip");
        execute_command("rm -rf " + target_folder + "/spitfire-minimal.zip");
        execute_command("rm -rf " + target_folder + "/opt/tweaksspitfire.sh");

        // Fix user-places.xbel
        std::string cmd = "ls -1 " + target_folder + "/home | grep -v '^\\.' | head -1";
        FILE* pipe = popen(cmd.c_str(), "r");
        if (pipe) {
            char buffer[128];
            std::string home_folder;
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                home_folder = buffer;
                home_folder.erase(std::remove(home_folder.begin(), home_folder.end(), '\n'), home_folder.end());
            }
            pclose(pipe);
            
            if (!home_folder.empty()) {
                std::string user_places_file = target_folder + "/home/" + home_folder + "/.local/share/user-places.xbel";
                std::string sed_cmd = "sed -i 's/spitfire/" + home_folder + "/g' " + user_places_file;
                execute_command(sed_cmd);
            }
        }

        // Cleanup mounts
        execute_command("umount " + target_folder + "/dev/pts");
        execute_command("umount " + target_folder + "/dev");
        execute_command("umount " + target_folder + "/proc");
        execute_command("umount " + target_folder + "/sys");
        execute_command("umount " + target_folder + "/run");

        std::cout << COLOR_GREEN << "Spitfire CKGE Minimal installation completed in: " << target_folder << COLOR_RESET << std::endl;
    }
};

int main() {
    SpitfireFolderInstaller installer;
    installer.run();
    return 0;
}
