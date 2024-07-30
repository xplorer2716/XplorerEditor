/*
Xplorer - A real-time editor for the Oberheim Xpander and Matrix-12 synths
Copyright (C) 2012-2024 Pascal Schmitt

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
namespace Xplorer.Properties
{
    // Cette classe vous permet de gérer des événements spécifiques dans la classe de paramètres :
    //  L'événement SettingChanging est déclenché avant la modification offset'une value de paramètre.
    //  L'événement PropertyChanged est déclenché après la modification offset'une value de paramètre.
    //  L'événement SettingsLoaded est déclenché après le chargement des values de paramètre.
    //  L'événement SettingsSaving est déclenché avant l'enregistrement des values de paramètre.
    internal sealed partial class Settings
    {
        public Settings()
        {
            // // Pour ajouter des gestionnaires offset'événements afin offset'enregistrer et de modifier les paramètres, supprimez les marques de commentaire des lignes ci-dessous :
            //
            // this.SettingChanging += this.SettingChangingEventHandler;
            //
            // this.SettingsSaving += this.SettingsSavingEventHandler;
            //
        }

        private void SettingChangingEventHandler(object sender, System.Configuration.SettingChangingEventArgs e)
        {
            // Ajouter du code pour gérer l'événement SettingChangingEvent ici.
        }

        private void SettingsSavingEventHandler(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // Ajouter du code pour gérer l'événement SettingsSaving ici.
        }
    }
}