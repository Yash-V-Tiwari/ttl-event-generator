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


TTLEventGenerator::TTLEventGenerator()
    : GenericProcessor("TTL Event Generator")
{
   addFloatParameter(Parameter::GLOBAL_SCOPE,
                     "interval",
                     "Interval (in ms)",
                     1000.0f,
                     0.0f,
                     5000.0f,
                     50.0f);

   StringArray outputs;
   for(int i = 1; i <= 8; i++) {
      outputs.add(String(i));
   }

   addCategoricalParameter(Parameter::GLOBAL_SCOPE,
                           "ttl_line",
                           "Event output line",
                           outputs,
                           0);
   
   addStringParameter(Parameter::GLOBAL_SCOPE,
                        "manual_trigger",
                        "Used to notify processor of manually triggered TTL events",
                        String());
}


TTLEventGenerator::~TTLEventGenerator()
{

}


AudioProcessorEditor* TTLEventGenerator::createEditor()
{
    editor = std::make_unique<TTLEventGeneratorEditor>(this);
    return editor.get();
}

bool TTLEventGenerator::startAcquisition()
{
   counter = 0;
   state = false;

   return true;
}

void TTLEventGenerator::updateSettings()
{
    // create and add a TTL channel to the first data stream
    EventChannel::Settings settings{
            EventChannel::Type::TTL,
            "TTL Event Generator Output",
            "Default TTL event channel",
            "ttl.events",
            dataStreams[0]
    };

    ttlChannel = new EventChannel(settings);
    eventChannels.add(ttlChannel); // this pointer is now owned by the eventChannels array
    ttlChannel->addProcessor(processorInfo.get()); // make sure the channel knows about this processor   
}


void TTLEventGenerator::process(AudioBuffer<float>& buffer)
{
// loop through the streams
   for (auto stream : getDataStreams())
   {
      // Only generate on/off event for the first data stream
      if(stream == getDataStreams()[0])
      {
         int totalSamples = getNumSamplesInBlock(stream->getStreamId());
         uint64 startSampleForBlock = getFirstSampleNumberForBlock(stream->getStreamId());

         int eventIntervalInSamples = (int) stream->getSampleRate();

         if (eventIntervalMs > 0)
         eventIntervalInSamples = (int) stream->getSampleRate() * eventIntervalMs / 2 / 1000;
         else
            eventIntervalInSamples = (int)stream->getSampleRate() * 100 / 2 / 1000;

         if (shouldTriggerEvent)
         {

            // add an ON event at the first sample.
            TTLEventPtr eventPtr = TTLEvent::createTTLEvent(ttlChannel,
               startSampleForBlock,
               outputLine, true);

            addEvent(eventPtr, 0);

            shouldTriggerEvent = false;
            eventWasTriggered = true;
            triggeredEventCounter = 0;
         }

         for (int i = 0; i < totalSamples; i++)
         {
            counter++;

            if (eventWasTriggered)
               triggeredEventCounter++;

            if (triggeredEventCounter == eventIntervalInSamples)
            {
               // add off event at the correct offset
               TTLEventPtr eventPtr = TTLEvent::createTTLEvent(ttlChannel,
                  startSampleForBlock + i,
                  outputLine, false);

               addEvent(eventPtr, i);

               eventWasTriggered = false;
               triggeredEventCounter = 0;
            }

            if (counter == eventIntervalInSamples && eventIntervalMs > 0)
            {

               state = !state;

               // add on or off event at the correct offset
               TTLEventPtr eventPtr = TTLEvent::createTTLEvent(ttlChannel,
                  startSampleForBlock + i,
                  outputLine, state);

               addEvent(eventPtr, i);

               counter = 0;

            }

            if (counter > eventIntervalInSamples)
               counter = 0;
         }
      }
   }
}


void TTLEventGenerator::handleTTLEvent(TTLEventPtr event)
{

}


void TTLEventGenerator::handleSpike(SpikePtr spike)
{

}


void TTLEventGenerator::handleBroadcastMessage(String message)
{

}


void TTLEventGenerator::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void TTLEventGenerator::loadCustomParametersFromXml(XmlElement* parentElement)
{

}

void TTLEventGenerator::parameterValueChanged(Parameter* param)
{
   if (param->getName().equalsIgnoreCase("manual_trigger"))
   {
      shouldTriggerEvent = true;
      LOGD("Event was manually triggered"); // log message
   }
   else if(param->getName().equalsIgnoreCase("interval"))
   {
      eventIntervalMs = (float)param->getValue();
   }
   else if(param->getName().equalsIgnoreCase("ttl_line"))
   {
      outputLine = (int)param->getValue();
   }
}
