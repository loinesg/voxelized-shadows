using UnityEditor;
using UnityEngine;
using System.Linq;
using System.IO;
using UnityEditor.SceneManagement;
using System;

using Object = UnityEngine.Object;

[Flags]
public enum ShaderFeature : uint
{
	None = 0,
	Texture = 1,
	NormalMap = 2,
	Specular = 4,
	Cutout = 8,
	Fog = 16,
}

public class SceneExporter
{
	public string SceneFolderPath;
	public string MaterialsFolderPath;
	public string MeshesFolderPath;
	public string TexturesFolderPath;

	[MenuItem("Custom/Export Scene")]
	private static void MenuItemPressed()
	{
		var pathSaveKey = "VoxelizedShadowsExportPath";
		var path = EditorUtility.SaveFolderPanel("Export Scene", EditorPrefs.GetString(pathSaveKey, ""), "");
		if(string.IsNullOrEmpty(path))
		{
			return;
		}

		var exporter = new SceneExporter();

		// Remember and set path
		EditorPrefs.SetString(pathSaveKey, path);
		exporter.SceneFolderPath = Path.Combine(path, "Scenes");
		exporter.MaterialsFolderPath = Path.Combine(path, "Materials");
		exporter.MeshesFolderPath = Path.Combine(path, "Meshes");
		exporter.TexturesFolderPath = Path.Combine(path, "Textures");

		// Check directories exist
		CreateDirectory(exporter.SceneFolderPath);
		CreateDirectory(exporter.MaterialsFolderPath);
		CreateDirectory(exporter.MeshesFolderPath);
		CreateDirectory(exporter.TexturesFolderPath);

		exporter.Run();
	}

	private static void CreateDirectory(string path)
	{
		if(!Directory.Exists(path))
			Directory.CreateDirectory(path);
	}

	public void Run()
	{
		var scenePath = EditorSceneManager.GetActiveScene().path;
		var dependencies = AssetDatabase.GetDependencies(scenePath);

		// Export all meshes the scene depends on
		foreach(var meshPath in dependencies.Where(d => d.EndsWith(".fbx")))
		{
			ExportMesh(meshPath);
		}

		// Export all textures the scene depends on
		foreach(var texturePath in dependencies.Where(d => (d.EndsWith(".psd") || d.EndsWith(".png"))))
		{
			ExportTexture(texturePath);
		}

		var camera = Object.FindObjectOfType<Camera>();
		var light = Object.FindObjectOfType<Light>();
		var renderers = Object.FindObjectsOfType<MeshRenderer>();
		ExportScene(camera, light, renderers);
	}
	
	private void ExportMesh(string meshPath)
	{
		var writer = new StringWriter();

		var mesh = AssetDatabase.LoadAssetAtPath<Mesh>(meshPath);

		var vertices = mesh.vertices;
		var normals = mesh.normals;
		var tangents = mesh.tangents;
		var texcoords = mesh.uv;
		for(var i = 0; i < vertices.Length; i++)
		{
			writer.WriteLine("vertex " + vertices[i].x + " " + vertices[i].y + " " + vertices[i].z);
			writer.WriteLine("normal " + normals[i].x + " " + normals[i].y + " " + normals[i].z);
			writer.WriteLine("tangent " + tangents[i].x + " " + tangents[i].y + " " + tangents[i].z + " " + tangents[i].w);
			writer.WriteLine("texcoord " + texcoords[i].x + " " + texcoords[i].y);
		}

		var triangles = mesh.triangles;
		for(var i = 0; i < triangles.Length / 3; i++)
		{
			writer.WriteLine("triangle " + triangles[i*3] + " " + triangles[i*3+1] + " " + triangles[i*3+2]);
		}

		var fileName = mesh.name + ".mesh";
		var filePath = Path.Combine(MeshesFolderPath, fileName);
		var fileContents = writer.ToString().Trim();
		File.WriteAllText(filePath, fileContents);
	}

	private void ExportTexture(string texturePath)
	{
		// Must be readable and uncompressed to extract .png
		var importer = TextureImporter.GetAtPath(texturePath) as TextureImporter;
		if(!importer.isReadable || importer.textureCompression != TextureImporterCompression.Uncompressed)
		{
			importer.isReadable = true;
			importer.textureCompression = TextureImporterCompression.Uncompressed;
			importer.SaveAndReimport();
		}

		// Disable normal map formats
		var inNormalMapFormat = importer.textureType == TextureImporterType.NormalMap;
		if(inNormalMapFormat)
		{
			importer.textureType = TextureImporterType.Default;
			importer.SaveAndReimport();
		}

		// Export as .png
		var texture = AssetDatabase.LoadAssetAtPath<Texture2D>(texturePath);
		var fileName = texture.name + ".png";
		var filePath = Path.Combine(TexturesFolderPath, fileName);
		var fileContents = texture.EncodeToPNG();
		File.WriteAllBytes(filePath, fileContents);

		// Switch back to normal map format (if it was disabled)
		if(inNormalMapFormat)
		{
			importer.textureType = TextureImporterType.NormalMap;
			importer.SaveAndReimport();
		}
	}

	private void ExportScene(Camera camera, Light light, MeshRenderer[] renderers)
	{
		Array.Sort(renderers.Select(r => r.name).ToArray(), renderers);

		var writer = new StringWriter();

		WriteSceneCamera(writer, camera);
		writer.WriteLine();
		
	 	WriteSceneLight(writer, light);
		writer.WriteLine();

		Array.Sort(renderers, SortMeshRenderers);
		foreach(var renderer in renderers)
		{
			WriteSceneMeshRenderer(writer, renderer);
			writer.WriteLine();
		}

		var fileName = EditorSceneManager.GetActiveScene().name + ".scene";
		var filePath = Path.Combine(SceneFolderPath, fileName);
		var fileContents = writer.ToString().Trim();
		File.WriteAllText(filePath, fileContents);
	}
	
	private int SortMeshRenderers(MeshRenderer a, MeshRenderer b)
	{
		// Get the features on each renderer
		var aFeatures = (int)GetMaterialFeatures(a.sharedMaterial);
		var bFeatures = (int)GetMaterialFeatures(b.sharedMaterial);

		// Sort by features
		return aFeatures - bFeatures;
	}

	private void WriteSceneTransform(StringWriter writer, Transform transform)
	{
		var position = transform.position;
		var rotation = transform.eulerAngles;
		var scale = transform.lossyScale;
		writer.Write(position.x + " " + position.y + " " + position.z + "   ");
		writer.Write(rotation.x + " " + rotation.y + " " + rotation.z + "   ");
		writer.Write(scale.x + " " + scale.y + " " + scale.z);
		writer.WriteLine();
	}

	private void WriteSceneCamera(StringWriter writer, Camera camera)
	{
		var fov = camera.fieldOfView;
		var nearPlane = camera.nearClipPlane;
		var farPlane = camera.farClipPlane;
		writer.WriteLine("camera " + fov + " " + nearPlane + " " + farPlane);

		WriteSceneTransform(writer, camera.transform);
	}

	private void WriteSceneLight(StringWriter writer, Light light)
	{
		var color = light.color * light.intensity;
		var ambient = RenderSettings.ambientLight * RenderSettings.ambientIntensity;
		writer.WriteLine("light " + color.r + " " + color.g + " " + color.b + " " + ambient.r + " " + ambient.g + " " + ambient.b);

		WriteSceneTransform(writer, light.transform);
	}

	private void WriteSceneMeshRenderer(StringWriter writer, MeshRenderer renderer)
	{
		var mesh = renderer.GetComponent<MeshFilter>().sharedMesh;
		var meshName = mesh.name + ".mesh";
		var material = renderer.sharedMaterial;
		var textureName = material.mainTexture.name + ".png";
		var normalMapName = material.GetTexture("_BumpMap").name + ".png";
		var features = GetMaterialFeatures(material);

		writer.WriteLine("mesh " + meshName + " " + features + " " + textureName + " " + normalMapName);
		WriteSceneTransform(writer, renderer.transform);

		// Write any attached animation component immediately after the renderer
		var animation = renderer.GetComponent<MeshAnimation>();
		if(animation != null)
		{
			WriteMeshAnimation(writer, animation);
		}
	}

	private uint GetMaterialFeatures(Material mat)
	{
		var features = ShaderFeature.None;
		if (mat.mainTexture != null) features |= ShaderFeature.Texture;
		if (mat.shader.name.Contains("Bump")) features |= ShaderFeature.NormalMap;
		if (mat.shader.name.Contains("Specular")) features |= ShaderFeature.Specular;
		if (mat.shader.name.Contains("Transparent")) features |= ShaderFeature.Cutout;
		features |= ShaderFeature.Fog;

		return (uint)features;
	}

	private void WriteMeshAnimation(StringWriter writer, MeshAnimation animation)
	{
		var startDelay = animation.StartDelay;
		var resetTime = animation.ResetTime;
		var rotationSpeed = animation.RotationSpeed;
		var translationSpeed = animation.TranslationSpeed;
		writer.WriteLine("animation " + startDelay + " " + resetTime
		+ " " + rotationSpeed.x + " " + rotationSpeed.y + " " + rotationSpeed.z
		+ " " + translationSpeed.x + " " + translationSpeed.y + " " + translationSpeed.z);
	}
}
