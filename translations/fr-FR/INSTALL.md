> ReliefWin has no public support endpoint configured. Contact the maintainer of this checkout.

# Installer ReliefWin

> [English](../../INSTALL.md) | [中文](../zh-CN/INSTALL.md) | **Français** | [Español](../es-ES/INSTALL.md) | [日本語](../ja-JP/INSTALL.md) | [العربية](../ar-MA/INSTALL.md) | [Українська](../uk-UA/INSTALL.md)

## Compiler les images

Préparez l'environnement de compilation et configurez une compilation comme décrit dans [Compiler ReliefWin vous-même](README.md#compiler-reliefwin-vous-même). Compilez ensuite l'image dont vous avez besoin depuis le dossier de sortie, par exemple `output-Clang-amd64-debug` :

| Commande | Crée | Pour |
|---|---|---|
| `ninja bootcd` | `bootcd.iso` | Lancer ReliefWin depuis une image de CD, ou y démarrer l'installation de ReliefWin et installer ReliefWin sur un disque. |
| `ninja reliefwinimg` | `ReliefWin.img` | Démarrer un disque où ReliefWin est déjà installé. Il démarre en UEFI ou en BIOS. |
| `ninja reliefwinvhd` | `ReliefWin.vhd`, `ReliefWin.vhdx` | Utiliser le même disque préinstallé dans des machines virtuelles : le VHD dans VirtualBox et Hyper-V génération 1, le VHDX dans Hyper-V génération 2, qui n'accepte pas les disques VHD. |

Chaque cible compile d'abord tout ce dont elle a besoin, et les fichiers sont écrits dans le dossier de sortie.

## Installer depuis le CD d'installation

Démarrez `bootcd.iso` dans une machine virtuelle ou sur un PC, et suivez les instructions de l'installation de ReliefWin. L'installation affiche les disques comme le programme d'installation de Windows, et peut créer, supprimer, formater, réduire et étendre des partitions. Elle installe ReliefWin sur NTFS et crée la partition système EFI dont il a besoin.

## Utiliser le disque préinstallé

`ReliefWin.img` ne nécessite aucune installation. Lancez-le dans une machine virtuelle comme QEMU, ou écrivez-le sur une clé USB ou une carte SD et démarrez la machine dessus. Utilisez `ReliefWin.vhd` pour les machines virtuelles qui acceptent les disques VHD, et `ReliefWin.vhdx` pour les machines virtuelles Hyper-V génération 2, avec le démarrage sécurisé désactivé.

Le disque préinstallé contient aussi l'installation de ReliefWin. Lancez `C:\<architecture>\reliefwin.exe`, par exemple `C:\arm64\reliefwin.exe`, pour installer ReliefWin sur un autre disque depuis le système en cours d'exécution.

## Aide

En cas de problème, signalez-le dans les Issues en précisant l'architecture, le type de compilation, le commit que vous avez compilé et les étapes qui mènent au problème. Les guides et les notes se trouvent dans le Wiki, et vous pouvez interroger l'équipe sur Discord ou sur Matrix.
