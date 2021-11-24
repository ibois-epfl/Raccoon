#include "StdAfx.h"
#include "joinery_solver_rhino7PlugIn.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>

#include <chrono>
using namespace std::chrono;

#pragma region command_mesh_difference command

CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> Convert_RhinoMeshToCGAL(const ON_Mesh* meshRhino0_) {



    ON_Mesh* meshRhino0 = RhinoWeldMesh(*meshRhino0_, ON_PI);
    meshRhino0->ConvertQuadsToTriangles();
    meshRhino0->CombineIdenticalVertices(true, true);
    meshRhino0->CullUnusedVertices();
    meshRhino0->Cleanup(true);
    meshRhino0->CullDegenerateFaces();
    meshRhino0->CullUnusedVertices();

    CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh1; //typedef CGAL::Surface_mesh<K::Point_3>             Mesh; typedef CGAL::Exact_predicates_inexact_constructions_kernel K;


    for (int i = 0; i < meshRhino0->m_V.Count(); i++) {
        auto x = meshRhino0->m_V[i].x;
        auto y = meshRhino0->m_V[i].y;
        auto z = meshRhino0->m_V[i].z;
        auto v = CGAL::Epick::Point_3(x, y, z);
        mesh1.add_vertex(v);
    }

    for (int i = 0; i < meshRhino0->m_F.Count(); i++) {
        auto x = meshRhino0->m_F[i].vi[0];
        auto y = meshRhino0->m_F[i].vi[1];
        auto z = meshRhino0->m_F[i].vi[2];
        mesh1.add_face(CGAL::SM_Vertex_index(x), CGAL::SM_Vertex_index(y), CGAL::SM_Vertex_index(z));
    }

    delete meshRhino0;

    return mesh1;

}

////https://stackoverflow.com/questions/46808246/cgal-get-face-data-from-surface-mesh

ON_Mesh Convert_CGALMeshToRhino(CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>  out) {

    ON_Mesh meshRebuilt(out.num_faces(), out.number_of_vertices(), false, false);

    for (auto vi : out.vertices()) {
        auto pt = out.point(vi);
        meshRebuilt.SetVertex(vi.idx(), ON_3dPoint(pt.x(), pt.y(), pt.z()));
    }

    //Get face indices ...

    for (auto face_index : out.faces()) {
        std::vector<uint32_t> indices;
        CGAL::Vertex_around_face_circulator<CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3>> vcirc(out.halfedge(face_index), out), done(vcirc);
        do indices.push_back(*vcirc++); while (vcirc != done);
        meshRebuilt.SetTriangle(face_index.idx(), indices[0], indices[1], indices[2]);

    }

    return meshRebuilt;

}

class command_mesh_difference : public CRhinoCommand {
public:

	command_mesh_difference() = default;
	~command_mesh_difference() = default;

	UUID CommandUUID() override {
		static const GUID joinery_solver_rhino7Command_UUID =
		{ 0x6f0776fb, 0x2e6b, 0x471f, { 0x9a, 0xd7, 0x38, 0x72, 0x3c, 0xc0, 0xfc, 0xa7 } };
		return joinery_solver_rhino7Command_UUID;
	}

	const wchar_t* EnglishCommandName() override { return L"joinery_solver_mesh_difference"; }
	CRhinoCommand::result RunCommand(const CRhinoCommandContext& context) override;
};



static class command_mesh_difference thejoinery_solver_rhino7Command;

static int RhinoFindOrCreateLayer(CRhinoDoc& doc, const wchar_t* layer_name) {
	if (nullptr == layer_name || 0 == layer_name[0])
		return ON_UNSET_INT_INDEX;

	int layer_index = doc.m_layer_table.FindLayerFromFullPathName(layer_name, ON_UNSET_INT_INDEX);
	if (layer_index == ON_UNSET_INT_INDEX) {
		ON_Layer layer;
		layer.SetName(layer_name);
		layer.SetColor(ON_Color(255, 0, 0));
		layer.SetPlotColor(ON_Color(255, 0, 0));
		layer_index = doc.m_layer_table.AddLayer(layer);
	}

	return layer_index;
}

CRhinoCommand::result command_mesh_difference::RunCommand(const CRhinoCommandContext& context) {

    ///////////////////////////////////////////////////////////////////////////////////
    //Get Mesh
    ///////////////////////////////////////////////////////////////////////////////////

    CRhinoGetObject go;
    go.SetCommandPrompt(L"Select 2 meshes");
    go.SetGeometryFilter(CRhinoGetObject::mesh_object);
    go.GetObjects(2, 2);


    if (go.CommandResult() != CRhinoCommand::success) return CRhinoCommand::failure;

    //Once pointcloud is selected get parameters
    const ON_Mesh* meshRhino0_ = go.Object(0).Mesh();
    const ON_Mesh* meshRhino1_ = go.Object(1).Mesh();

    /////////////////////////////////////////////////////////////////////
    //Call Main Method
    /////////////////////////////////////////////////////////////////////
    RhinoApp().Print("============================================================================== CPP + \n");
    auto start = high_resolution_clock::now();

    ON_Mesh output(0,0,true,true);
    if (meshRhino0_ && meshRhino1_ && meshRhino0_->IsSolid() && meshRhino1_->IsSolid()) {

        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh1 = Convert_RhinoMeshToCGAL(meshRhino0_);
        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> mesh2 = Convert_RhinoMeshToCGAL(meshRhino1_);
        


        ////////////////////////////////////////////////////////////////
        // Check if Converted meshes are valid
        ////////////////////////////////////////////////////////////////
        if (!mesh1.is_valid(false) || !mesh2.is_valid(false)) {
            RhinoApp().Print(L"Invalid Mesh\n");
            return CRhinoCommand::failure;
        }


        ////////////////////////////////////////////////////////////////
        // Perform CGAL Boolean
        ////////////////////////////////////////////////////////////////
        CGAL::Surface_mesh<CGAL::Exact_predicates_inexact_constructions_kernel::Point_3> outCGAL;
        bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_difference(mesh1, mesh2, outCGAL);
        //bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_intersection(mesh1,mesh2, out);
        //bool valid_union = CGAL::Polygon_mesh_processing::corefine_and_compute_union(mesh1,mesh2, out);

        if (valid_union)
            RhinoApp().Print(L"Union was successfully computed\n");
        else {
            RhinoApp().Print(L"Union could not be computed\n");
            return CRhinoCommand::failure;
        }

       // ////////////////////////////////////////////////////////////////
       // // CGAL Mesh to Rhino Mesh
       // ////////////////////////////////////////////////////////////////
       output = Convert_CGALMeshToRhino(outCGAL);
       //////https://stackoverflow.com/questions/46808246/cgal-get-face-data-from-surface-mesh
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    RhinoApp().Print("==================================== %d ms ==================================== \n", duration);
    RhinoApp().Print("============================================================================== CPP - \n");

    /////////////////////////////////////////////////////////////////////
    //Output
    /////////////////////////////////////////////////////////////////////



    //Create layer
    const wchar_t* layer_name = L"joinery_solver_mesh";
    int layer_index = RhinoFindOrCreateLayer(context.m_doc, layer_name);
    if (layer_index == ON_UNSET_INT_INDEX)
        return CRhinoCommand::failure;

    ON_3dmObjectAttributes attributes;
    context.m_doc.GetDefaultObjectAttributes(attributes);
    attributes.m_layer_index = layer_index;


    ////////////////////////////////////////////////////////////////
    // Add Rhino Mesh and delete input meshes
    ////////////////////////////////////////////////////////////////

    if (output.IsValid()) {
        auto meshObjBaked = context.m_doc.AddMeshObject(output, &attributes);

        context.m_doc.DeleteObject(go.Object(0));
        context.m_doc.DeleteObject(go.Object(1));
        context.m_doc.Redraw();
    }

    context.m_doc.Redraw();



    return CRhinoCommand::success;
}

#pragma endregion

