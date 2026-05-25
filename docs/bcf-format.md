# BCF Format Overview

BCF-XML (BIM Collaboration Format) is an open ISO/buildingSMART standard for exchanging coordination issues between BIM authoring tools without sharing the model itself. GDBCF implements **BCF 3.0**.

---

## Container Layout

A `.bcf` file is a ZIP archive. GDBCF reads and writes this layout:

```
coordination.bcf (ZIP)
│
├─ bcf.version                   # Version declaration
├─ extensions.xml                # Allowed enum values
├─ project.bcfp                  # (optional) Project ID and name
├─ documents.xml                 # (optional) List of embedded documents
│
├─ {topic-guid}/
│  ├─ markup.bcf                 # Topic metadata, comments, viewpoint refs
│  ├─ {viewpoint-guid}.bcfv      # Viewpoint: camera + component visibility
│  ├─ {viewpoint-guid}.png       # Snapshot image (PNG or JPEG)
│  └─ ...
│
├─ {topic-guid}/
│  └─ ...
│
└─ {document-guid}               # Embedded document bytes (optional)
```

---

## bcf.version

```xml
<?xml version="1.0" encoding="utf-8"?>
<Version VersionId="3.0"/>
```

GDBCF sets `BCFProject.version = "3.0"` and warns (does not fail) when opening files with other version IDs.

---

## project.bcfp

```xml
<?xml version="1.0" encoding="utf-8"?>
<ProjectInfo>
  <Project ProjectId="a3b4c5d6-...">
    <Name>Site Coordination - Block A</Name>
  </Project>
</ProjectInfo>
```

Maps to `BCFProject.project_info` (`BCFProjectInfo`).

---

## extensions.xml

```xml
<?xml version="1.0" encoding="utf-8"?>
<Extensions>
  <TopicTypes>
    <TopicType>CLASH</TopicType>
    <TopicType>WARNING</TopicType>
  </TopicTypes>
  <TopicStatuses>
    <TopicStatus>OPEN</TopicStatus>
    <TopicStatus>CLOSED</TopicStatus>
  </TopicStatuses>
  <Priorities>
    <Priority>HIGH</Priority>
    <Priority>LOW</Priority>
  </Priorities>
</Extensions>
```

Maps to `BCFProject.extensions` (`BCFExtensions`). `create_project()` populates this with sensible defaults.

---

## markup.bcf

The main topic file. Contains metadata, comments, and references to viewpoint files.

```xml
<?xml version="1.0" encoding="utf-8"?>
<Markup>
  <Topic Guid="e1f2..." TopicType="CLASH" TopicStatus="OPEN">
    <Title>Pipe clashes with primary beam at grid C-4</Title>
    <CreationDate>2026-05-25T09:00:00Z</CreationDate>
    <CreationAuthor>alice@office.com</CreationAuthor>
    <Comments>
      <Comment Guid="c3d4...">
        <Date>2026-05-25T09:05:00Z</Date>
        <Author>alice@office.com</Author>
        <Comment>Confirmed in site visit. Needs re-routing.</Comment>
      </Comment>
    </Comments>
    <Viewpoints>
      <ViewPoint Guid="v5a6...">
        <Viewpoint>v5a6.bcfv</Viewpoint>
        <Snapshot>v5a6.png</Snapshot>
      </ViewPoint>
    </Viewpoints>
  </Topic>
</Markup>
```

Maps to `BCFTopic`, `BCFComment`, and `BCFViewpointRef`.

---

## Viewpoint file (.bcfv)

```xml
<?xml version="1.0" encoding="utf-8"?>
<VisualizationInfo Guid="v5a6...">
  <Components>
    <Selection>
      <Component IfcGuid="0ABC123..."/>
    </Selection>
    <Visibility DefaultVisibility="true">
      <ViewSetupHints SpacesVisible="false"
                      SpaceBoundariesVisible="false"
                      OpeningsVisible="false"/>
      <Exceptions>
        <Component IfcGuid="0DEF456..."/>
      </Exceptions>
    </Visibility>
  </Components>
  <PerspectiveCamera>
    <CameraViewPoint><X>10.5</X><Y>3.2</Y><Z>1.8</Z></CameraViewPoint>
    <CameraDirection><X>-0.7</X><Y>0.0</Y><Z>-0.7</Z></CameraDirection>
    <CameraUpVector><X>0.0</X><Y>1.0</Y><Z>0.0</Z></CameraUpVector>
    <FieldOfView>60</FieldOfView>
    <AspectRatio>1.777</AspectRatio>
  </PerspectiveCamera>
</VisualizationInfo>
```

Maps to `BCFVisualizationInfo`, `BCFComponents`, `BCFComponent`, `BCFComponentVisibility`, and `BCFPerspectiveCamera`.

---

## IFC GUIDs

BCF component references use **compressed IFC GUIDs** (a 22-character base-64-like encoding), *not* standard UUID strings. When using the GDIFC bridge, `IFCNode.ifc_guid` is already in this compressed form so matching works directly.

---

## Coordinate System

BCF stores coordinates in the model's IFC coordinate system. If your IFC model was shifted to origin by GDIFC's `coordinate_to_origin` option you will need to apply the same offset when interpreting BCF camera positions.

---

## Further Reading

- [BCF-XML 3.0 Specification](https://github.com/buildingSMART/BCF-XML/tree/release_3_0) (GitHub)
- [BCF-XML Schemas](../BCFRepo/Schemas/) — XSD files bundled with this repo
