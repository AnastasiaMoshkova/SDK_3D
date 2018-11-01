/******************************************************************************\
* Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "LeapC.h"
#include "ExampleConnection.h"
#include <conio.h>

int64_t lastFrameID = 0; //The last frame received

int main(int argc, char** argv) {
  OpenConnection();
  while(!IsConnected)
    millisleep(100); //wait a bit to let the connection complete

  printf("Connected.\n");
  LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
  if(deviceProps)
    printf("Using device %s.\n", deviceProps->serial);

  LEAP_RECORDING recordingHandle;
  LEAP_RECORDING_PARAMETERS params;
  unsigned char key;
  //Open the recording for writing
  params.mode = eLeapRecordingFlags_Writing;
  eLeapRS result = LeapRecordingOpen(&recordingHandle, "leapRecording.lmt", params);
  if(LEAP_SUCCEEDED(result)){
    int frameCount = 0;
    //while(frameCount < 10){
	char ch;
	int code;
	while (true) {
		//ch = getchar();
		//code = static_cast<int>(ch);
		//if (ch == 'q') {
		if (kbhit()){
			//CloseConnection();
			//exit(0);
			break;
		}
      LEAP_TRACKING_EVENT *frame = GetFrame();
      if(frame && (frame->tracking_frame_id > lastFrameID)){
        lastFrameID = frame->tracking_frame_id;
        frameCount++;
        uint64_t dataWritten = 0;
        result = LeapRecordingWrite(recordingHandle, frame, &dataWritten);
        printf("Recorded %"PRIu64" bytes for frame %"PRIu64" with %i hands.\n", dataWritten, frame->tracking_frame_id, frame->nHands);

		
      }
    }
    result = LeapRecordingClose(&recordingHandle);
    if(!LEAP_SUCCEEDED(result))
      printf("Failed to close recording: %s\n", ResultString(result));

    //Reopen the recording for reading
    params.mode = eLeapRecordingFlags_Reading;
    result = LeapRecordingOpen(&recordingHandle, "leapRecording.lmt", params);
    if(LEAP_SUCCEEDED(result)){
      LEAP_TRACKING_EVENT *frame = 0;
      while(frameCount-- > 0){
        uint64_t nextFrameSize = 0;
        result = LeapRecordingReadSize(recordingHandle, &nextFrameSize);
        if(!LEAP_SUCCEEDED(result))
          printf("Couldn't get next frame size: %s\n", ResultString(result));
        if(nextFrameSize > 0){
          frame = (LEAP_TRACKING_EVENT *)malloc(nextFrameSize);
          result = LeapRecordingRead(recordingHandle, frame, nextFrameSize);
          if(LEAP_SUCCEEDED(result)){
            printf("Read frame %"PRIu64" with %i hands.\n", frame->tracking_frame_id, frame->nHands);

			//---------------
			FILE* file = fopen("hand_point3d_LM.txt", "a");
			//}

			static int i;
			i++;
			fprintf(file, "    frame %i.\n",
				i);

			printf("Frame %lli with %i hands.\n", (long long int)frame->info.frame_id, frame->nHands);

			for (uint32_t h = 0; h < frame->nHands; h++) {
				LEAP_HAND* hand = &frame->pHands[h];

				printf("    Hand id %i is a %s hand with position (%f, %f, %f).\n",
					hand->id,
					(hand->type == eLeapHandType_Left ? "left" : "right"),
					hand->palm.position.x,
					hand->palm.position.y,
					hand->palm.position.z);
				fprintf(file, "    Hand id %i is a %s hand with position (%f, %f, %f).\n",
					hand->id,
					(hand->type == eLeapHandType_Left ? "left" : "right"),
					hand->palm.position.x,
					hand->palm.position.y,
					hand->palm.position.z);

				//-------------------
				for (int f = 0; f < 5; f++) {
					LEAP_DIGIT finger = hand->digits[f];
					printf("    Finger %i.\n",
						f);
					/*fprintf(file, "    Finger %i.\n",
					f);*/
					for (int b = 0; b < 4; b++) {
						LEAP_BONE bone = finger.bones[b];

						printf("    bone with position (%f, %f, %f, %f, %f, %f, %f, %f, %f).\n",
							bone.next_joint.x,
							bone.next_joint.y,
							bone.next_joint.z,
							bone.prev_joint.x,
							bone.prev_joint.y,
							bone.prev_joint.z,
							bone.rotation.x,
							bone.rotation.y,
							bone.rotation.z);
						fprintf(file, "    bone with position (%f, %f, %f, %f, %f, %f, %f, %f, %f).\n",
							bone.next_joint.x,
							bone.next_joint.y,
							bone.next_joint.z,
							bone.prev_joint.x,
							bone.prev_joint.y,
							bone.prev_joint.z,
							bone.rotation.x,
							bone.rotation.y,
							bone.rotation.z);


					}
				}
				//--------------------
			}
			//fprintf(f, "%i", i);
			fclose(file);
			//---------------
          } else {
            printf("Could not read frame: %s\n", ResultString(result));
          }
        }
      }
      result = LeapRecordingClose(&recordingHandle);
      if(!LEAP_SUCCEEDED(result))
        printf("Failed to close recording: %s\n", ResultString(result));
    } else {
      printf("Failed to open recording for reading: %s\n", ResultString(result));
    }
  } else {
    printf("Failed to open recording for writing: %s\n", ResultString(result));
  }
  return 0;
}
//End-of-Sample
