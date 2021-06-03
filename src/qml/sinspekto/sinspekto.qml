import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.4
import QtQuick.Layouts 1.11
import QtCharts 2.2

import fkin.Dds 1.0

QtObject
{
//  property SystemPalette palette: SystemPalette { id: darkPalette; colorGroup: SystemPalette.Active }

  property var applicationWindow: ApplicationWindow
  {
    id: window;
    title: qsTr("FKIN Inspector");
    width: 1920; //Screen.width;
    height: 1080; //Screen.height;
    font : defaultFont;
    visible: false;

    //FontLoader { id: theFont; source: "fonts/PublicSans-VariableFont_wght.ttf" }
    readonly property string fontFamily: "Public Sans"; // added in .cpp

    property int chartTheme: Material.theme == Material.Light ?
      ChartView.ChartThemeLight : ChartView.ChartThemeDark;

    property font defaultFont: Qt.font({ family: fontFamily, pointSize: 16, weight: Font.Bold } );
    property font plotFont:    Qt.font({ family: fontFamily, pointSize: 12, weight: Font.Bold } );
    property font numberFont:  Qt.font({ family: fontFamily, pointSize: 20, weight: Font.Bold } );
    property font largeNumberFont: Qt.font({ family: fontFamily, pointSize: 26, weight: Font.Bold } );

    FkinDdsSetup
    {
      // currently do not know how to let this be in another file and still get access to
      // them in sibling components, which are included in window.
      QtToDds
      {
        id: ddsParticipant;
        readonly property int domain: 0;

        Component.onCompleted:
        {
          init(domain);

          // Commands
          ddsCmdVessel.init(ddsParticipant,      "fkinCmd", "Vessel",     "fkinCmdResp", 2000);
          ddsCmdFishSchool.init(ddsParticipant,  "fkinCmd", "Fish",       "fkinCmdResp", 2000);
          ddsCmdLeadline.init(ddsParticipant,    "fkinCmd", "Leadline",  "fkinCmdResp", 2000);
          ddsCmdEstimator.init(ddsParticipant,   "fkinCmd", "Estimator",  "fkinCmdResp", 2000);
          ddsCmdPlanner.init(ddsParticipant,     "fkinCmd", "Planner",    "fkinCmdResp", 2000);

          // Notifications
          ddsVesselState.init(ddsParticipant,    "fkinStateNotification", "Vessel");
          ddsFishSchoolState.init(ddsParticipant,"fkinStateNotification", "Fish");
          ddsLeadlineState.init(ddsParticipant,  "fkinStateNotification", "Leadline");
          ddsEstimatorState.init(ddsParticipant, "fkinStateNotification", "Estimator");
          ddsPlannerState.init(ddsParticipant,   "fkinStateNotification", "Planner");

          // Controller signals
          ddsVesselCtrl.init(ddsParticipant,     "fkinVesselCtrl", "Vessel", Qt.vector3d(0,0,0), false);
          ddsFishCtrl.init(ddsParticipant,       "fkinFishCtrl",   "Fish",   Qt.vector3d(0,0,0), false);
          ddsCurrentCtrl.init(ddsParticipant,    "fkinCurrent",   "Current",   0, false);

          // Initial conditions
          ddsVesselInit.init(ddsParticipant,     "fkinPositionCourse", "Vessel", Qt.vector3d(0,0,0), false);
          ddsFishInitPos.init(ddsParticipant,    "fkinPosition",       "FishInit",   Qt.vector3d(0,0,0), false);
          ddsFishInitEuler.init(ddsParticipant,  "fkinEuler",          "FishInit",   Qt.vector3d(0,0,0), false);

          // Output from algorithms into buffer
          ddsVessel.init(ddsParticipant,  "fkinKinematics2D", "Vessel", 300, false);
          ddsFish.init(ddsParticipant,    "fkinKinematics6D", "Fish", 300, false);
          ddsLeadline.init(ddsParticipant,"fkinBatchIdVec1d", "Leadline", 301, true);

          // Newest sample
          ddsVesselNow.init(ddsParticipant, "fkinKinematics2D", "Vessel");
          ddsFishNow.init(ddsParticipant, "fkinKinematics6D", "Fish");

        }
      }

      // Commands to algorithms
      DdsCommandPublisher   { id: ddsCmdVessel;      }
      DdsCommandPublisher   { id: ddsCmdFishSchool;  }
      DdsCommandPublisher   { id: ddsCmdLeadline;    }
      DdsCommandPublisher   { id: ddsCmdEstimator;   }
      DdsCommandPublisher   { id: ddsCmdPlanner;     }

      // Notifications from algorithms
      DdsStateNotification  { id: ddsVesselState;    }
      DdsStateNotification  { id: ddsFishSchoolState;}
      DdsStateNotification  { id: ddsLeadlineState;  }
      DdsStateNotification  { id: ddsEstimatorState; }
      DdsStateNotification  { id: ddsPlannerState;   }

      // Controller signals to simulators
      DdsIdVec3dPublisher   { id: ddsVesselCtrl;     }
      DdsIdVec3dPublisher   { id: ddsFishCtrl;       }
      DdsIdVec1dPublisher   { id: ddsCurrentCtrl;    }

      // Initial conditions to simulators
      DdsIdVec3dPublisher   { id: ddsVesselInit;     }
      DdsIdVec3dPublisher   { id: ddsFishInitPos;    }
      DdsIdVec3dPublisher   { id: ddsFishInitEuler;  }

      // Output from algorithms
      DdsKinematics2DBuffer      { id: ddsVessel;    }
      DdsKinematics6DBuffer      { id: ddsFish;      }
      DdsIdVec1dBuffer           { id: ddsLeadline;  }
      DdsKinematics2DSubscriber  { id: ddsVesselNow; }
      DdsKinematics6DSubscriber  { id: ddsFishNow;   }

      // ddsEstimator
      // ddsPlanner
    }

    footer: FkinFooter{ id: fkinFooter; }
    header: TabBar
    {
      id: navbar
      width: parent.width
      TabButton { text: qsTr("Control components"); }
      currentIndex: 0;
    }

    StackLayout
    {
      id: view
      currentIndex: navbar.currentIndex
      width: parent.width

      Item { FkinControlComponents {} } // About tab

    }
    Component.onCompleted: { splashWindow.timerRun = true; }
  }

  property var splashWindow: Splash {
    onTimeout:
    {
      splashWindow.visible = false;
      applicationWindow.visible = true;
      //onTimeout: applicationWindow.visibility = "FullScreen";
    }
  }
}
