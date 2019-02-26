#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>
#include <osgViewer/Viewer>

#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/PlaceNode>

#include <osgEarthUtil/ClusterNode>

using namespace osgEarth;
using namespace osgEarth::Util;
using namespace osgEarth::Annotation;

void makeCircles(MapNode* mapNode, unsigned int count, const GeoExtent& extent,
                 osg::NodeList& nodes) {
  Style circleStyle;
  LineSymbol* ls = circleStyle.getOrCreate<LineSymbol>();

  ls->stroke()->color() = osgEarth::Symbology::Color::Orange;
  ls->stroke()->width() = 3;
  ls->stroke()->smooth() = true;

  AltitudeSymbol* as = circleStyle.getOrCreateSymbol<AltitudeSymbol>();
  as->clamping() = osgEarth::AltitudeSymbol::CLAMP_TO_TERRAIN;
  as->technique() = osgEarth::AltitudeSymbol::TECHNIQUE_DRAPE;

  const SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();
  {
    for (unsigned int i = 0; i < count; i++) {
      double lat =
          extent.yMin() + extent.height() * (rand() * 1.0) / (RAND_MAX - 1);
      double lon =
          extent.xMin() + extent.width() * (rand() * 1.0) / (RAND_MAX - 1);
      CircleNode* circle = new CircleNode();
      circle->set(GeoPoint(geoSRS, lon, lat, 0.0),
                  Distance(10, Units::KILOMETERS), circleStyle);
      circle->setMapNode(mapNode);
      circle->setDynamic(true);
      nodes.push_back(circle);
    }
  }
}

int main(int argc, char** argv) {
  osg::ArgumentParser arguments(&argc, argv);

  osgViewer::Viewer viewer(arguments);

  viewer.setCameraManipulator(new EarthManipulator(arguments));

  viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);
  viewer.getCamera()->setNearFarRatio(0.0001);

  osg::Node* node = MapNodeHelper().load(arguments, &viewer);
  if (node) {
    MapNode* mapNode = MapNode::findMapNode(node);
    osg::NodeList nodes;

    GeoExtent extent(SpatialReference::create("wgs84"), -180, -90, 180, 90);

    makeCircles(mapNode, 1000, extent, nodes);

#if 1
    ClusterNode* clusterNode = new ClusterNode(mapNode);
    for (unsigned int i = 0; i < nodes.size(); i++) {
      clusterNode->addNode(nodes[i].get());
    }
    mapNode->addChild(clusterNode);
#else
    for (unsigned int i = 0; i < nodes.size(); i++) {
      mapNode->addChild(nodes[i].get());
    }
#endif
    viewer.setSceneData(node);

    while (!viewer.done()) {
      viewer.frame();
    }
    return viewer.run();
  }

  return 0;
}
