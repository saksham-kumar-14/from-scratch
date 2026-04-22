import os
IGNORE_LIST = ['.git', '.github']

def get_projects():
    projects = []
    for item in sorted(os.listdir('.')):
        if os.path.isdir(item) and item not in IGNORE_LIST and not item.startswith('.'):
            projects.append(f"- **[{item}](./{item})**")
    
    return "\n".join(projects)

def update_readme():
    projects_md = get_projects()
    new_readme_content = f"""# Utilities from scratch 
{projects_md}
"""
    with open('README.md', 'w') as file:
        file.write(new_readme_content)

if __name__ == "__main__":
    update_readme()
    print("README.md updated.")
