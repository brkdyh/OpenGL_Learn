using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using UnityEditor.ProjectWindowCallback;
using UnityEngine;

public class ShaderTemplate
{
    [MenuItem("Assets/Create/Shader/Empty",false,90)]
    public static void CreateEmptyShader()
    {
        ProjectWindowUtil.StartNameEditingIfProjectWindowExists(0, 
            ScriptableObject.CreateInstance<CreareEmptyShaderAction>(), 
            GetSelectedPath() + "/NewEmptyShader.shader", 
            null,
            "Assets/Editor/Template/90-Shader-NewEmptyShader.shader.txt");
    }

    private static string GetSelectedPath()
    { //默认路径为Assets 
        string selectedPath = "Assets";
        //获取选中的资源 
        Object[] selection = Selection.GetFiltered(typeof(Object), SelectionMode.Assets);
        //遍历选中的资源以返回路径 
        foreach (Object obj in selection)
        {
            selectedPath = AssetDatabase.GetAssetPath(obj);
            if (!string.IsNullOrEmpty(selectedPath) && File.Exists(selectedPath))
            {
                selectedPath = Path.GetDirectoryName(selectedPath);
                break;
            }
        }
        return selectedPath;
    }
}

public class CreareEmptyShaderAction : EndNameEditAction
{
    public override void Action(int instanceId, string pathName, string resourceFile)
    {
        //File.create
        UnityEngine.Object obj = CreateAssetFormTemplate(pathName, resourceFile);
        //高亮显示该资源 
        ProjectWindowUtil.ShowCreatedAsset(obj);
    }

    internal static UnityEngine.Object CreateAssetFormTemplate(string pathName,string resourceFile)
    {
        //获取要创建资源的绝对路径 
        string fullName = Path.GetFullPath(pathName);
        //读取本地模版文件 
        StreamReader reader = new StreamReader(resourceFile); string content = reader.ReadToEnd();
        reader.Close();

        //获取资源的文件名 
        string fileName = Path.GetFileNameWithoutExtension(pathName);

        //Debug.LogError(content);
        //替换默认的文件名 
        content = content.Replace("#NAME", fileName);
        content = content.Replace("#SHADERTAB", "Brkdyh/" + fileName);
        //写入新文件 
        StreamWriter writer = new StreamWriter(fullName, false, System.Text.Encoding.UTF8);
        writer.Write(content);
        writer.Close();
        //刷新本地资源 
        AssetDatabase.ImportAsset(pathName);
        AssetDatabase.Refresh();
        return AssetDatabase.LoadAssetAtPath(pathName, typeof(UnityEngine.Object));
    }
}
