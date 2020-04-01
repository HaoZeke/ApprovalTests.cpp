import os

from scripts.utilities import write_file, read_file, get_file_name


def get_all_files(directory):
    all_files = []
    abs = os.path.abspath(directory)
    relative = get_file_name(abs)

    for root, directories, files in os.walk(directory):
        for file in files:
            if file.endswith('.h'):
                file_text = os.path.join(root, file)
                file_text = file_text.replace('./', relative + '/')
                all_files.append(file_text)
    all_files.sort()
    return all_files


class SingleHeaderFile(object):
    @staticmethod
    def create(directory):

        # 	public static List<File> getAllFiles(String directory) {
        # 		FileFilter filter = p -> p.getName().endsWith(".h") || p.getName().endsWith(".cpp");
        # 		File[] list = FileUtils.getRecursiveFileList(new File(directory), filter);
        # 		Arrays.sort(list);
        # 		return Arrays.asList(list);
        # 	}
        files = get_all_files(directory)
        files = SingleHeaderFile.sort_by_dependencies(files)
        output_file = os.path.abspath('ApprovalTests.hpp')

        includes = '\n'.join(map(lambda f: f'#include "{f}"', files))
        output = f'''
#ifndef APPROVALTESTS_CPP_APPROVALS_HPP
#define APPROVALTESTS_CPP_APPROVALS_HPP

// Created with:
//   cd ApprovalTests.cpp
//   ./scripts/create_simulated_single_header.sh

{includes}

#endif // APPROVALTESTS_CPP_APPROVALS_HPP

'''
        write_file(output_file, output)
        return output_file

    @staticmethod
    def sort_by_dependencies(files):
        parts = list(map(SingleHeaderFile.get_parts, files))
        return list(map(lambda p: p['file'], SingleHeaderFile.sort_parts_by_dependencies(parts)))

    @staticmethod
    def get_parts(file):
        content = read_file(os.path.join('..', file))
        lines = content.splitlines()
        include_lines = filter(lambda t: t.startswith('#include "'), lines)
        include_files = list(map(lambda i: i.replace('#include ', '').replace('"', ''), include_lines))
        include_files = list(map(lambda f:get_file_name(f), include_files))
        return {'file': file, 'include': include_files}

    @staticmethod
    def sort_parts_by_dependencies(parts):
        length = len(parts)
        for lower_index in range(length):
            lower = parts[lower_index]
            for higher_index in range(lower_index + 1, length):
                higher = parts[higher_index]
                if SingleHeaderFile.depends_on(lower, higher):
                    parts.remove(higher)
                    parts.insert(0, higher)
                    return SingleHeaderFile.sort_parts_by_dependencies(parts)
        return parts

    @staticmethod
    def depends_on(file1, file2):
        file2_name = get_file_name(file2['file'])
        return file2_name in file1['include']
