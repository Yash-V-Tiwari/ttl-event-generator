/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2022 Open Ephys

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "TTLEventGenerator.h"
#include "TTLEventGeneratorEditor.h"

TTLEventGeneratorEditor::TTLEventGeneratorEditor(GenericProcessor* parentNode)
    : GenericEditor(parentNode)
{

    desiredWidth = 150;

    addSliderParameterEditor("interval", 100, 25);
    addComboBoxParameterEditor("ttl_line", 10, 25);

    Parameter* manualTrigger = getProcessor()->getParameter("manual_trigger");
    addCustomParameterEditor(new ManualTriggerButton(manualTrigger), 60, 95);
}

ManualTriggerButton::ManualTriggerButton(Parameter* param)
     : ParameterEditor(param)
{
   triggerButton = std::make_unique<UtilityButton>("Trigger", Font("Fira Code", "Regular", 12.0f)); // button text, font to use
   triggerButton->addListener(this); // add listener to the button
   addAndMakeVisible(triggerButton.get());  // add the button to the editor and make it visible

   setBounds(0, 0, 70, 20); // set the bounds of custom parameter editor
}

void ManualTriggerButton::buttonClicked(Button* b)
{
   param->setNextValue(triggerButton->getLabel());
}

void ManualTriggerButton::resized()
{

   triggerButton->setBounds(0, 0, 70, 20);
}